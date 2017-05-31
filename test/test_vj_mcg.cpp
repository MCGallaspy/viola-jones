//
// Created by Mike on 5/17/2017.
//
#include "basic_bitmap.h"
#include "integral_image.h"
#include "weak_classifier.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace mcg;

std::pair<bitmap8, std::array<rect, 2>> from_bioid_files(std::istream& image_file, std::istream& eye_file);

template <typename func_t>
void test_runner(std::string&& m_name, func_t&& m_func)
{
    std::cout << "Running " << m_name << "... ";
    auto start = std::chrono::system_clock::now();
    m_func();
    using nanoseconds = std::chrono::nanoseconds;
    nanoseconds end = std::chrono::system_clock::now() - start;
    std::cout << "finished " << m_name << " in " << end.count() << "ns.\n";
}

#define MCG_RUN_TEST(test_name) \
    test_runner(#test_name, test_name);

void test_weak_classifier_with_bioid_dataset()
{
    static constexpr auto FILE_MAX = 1520;
    using ii_t = integral_image<bitmap8>;
    std::vector<std::pair<ii_t, std::array<rect, 2>>> files ;
    std::cout << "Reading files...\n";
    static constexpr std::size_t w = 12;
    for (auto i = 0; i < FILE_MAX; ++i)
    {
        std::stringstream sstr;
        sstr << "../bioid-faces/BioID_"
             << std::setw(4) << std::setfill('0') << i << ".pgm";
        std::ifstream in(sstr.str());
        assert(in);
        std::string tmp;
        int width, height, max_pixel_val;
        in >> tmp >> width >> height >> max_pixel_val;
        bitmap8 bm(width, height);
        bm.m_data.resize(width * height);
        for (auto y = 0; y < height; ++y)
        {
            for (auto x = 0; x < width; ++x)
            {
                const auto c = in.get();
                bm.at(x, y) = c;
            }
        }
        in.close();
        sstr.str("");
        sstr << "../bioid-faces/BioID_"
             << std::setw(4) << std::setfill('0') << i << ".eye";
        in.open(sstr.str());
        assert(in);
        std::string comment;
        std::getline(in, comment, '\n');
        int lx, ly, rx, ry;
        in >> lx >> ly >> rx >> ry;
        if (lx >= w && ly >= w && rx >= w && ry >= w) {
            rect left_eye {lx - w, ly - w, 2 * w, 2 * w},
                 right_eye{rx - w, ry - w, 2 * w, 2 * w};
            std::array<rect, 2> eyes{
                    std::move(left_eye),
                    std::move(right_eye)
            };
            files.push_back(std::make_pair(ii_t::create(bm), std::move(eyes)));
        } else {
            std::cerr << "Bad file " << i << "\n";
        }
    }
/*    haar_feature<ii_t, 24, 24> feature(
            std::vector<rect>{ rect{0, 0, w/3, w}, rect{2*w/3, 0, w/3, w} },
            std::vector<rect>{ rect{w/3, 0, w/3, w} }
    );*/
    haar_feature<ii_t, 24, 24> feature(
            std::vector<rect>{ rect{0, 0, w/2, w} },
            std::vector<rect>{ rect{w/2, 0, w/2, w} }
    );
    weak_classifier<ii_t, 24, 24> wc{std::move(feature)};
    using data_t = typename weak_classifier<ii_t, 24, 24>::data_t;
    data_t pos, neg;
    std::cout << "Preparing data...\n";
    for (const auto& pair : files)
    {
        for (const auto& eye : pair.second)
        {
            pos.push_back(pair.first.vectorize_window(eye));
        }
        neg.push_back(pair.first.vectorize_window(rect{0, 0, 24, 24}));
    }
    std::cout << "Using " << pos.size() << " pos. samples\n"
              << "\tand " << neg.size() << " neg. samples\n";
    std::cout << "Training...\n";
    wc.train(pos, neg);
    std::cout << "threshold: " << wc.threshold << ", " << (wc.parity ? "x < threshold" : "x > threshold")<< "\n";
    assert(wc.predict(files[0].first, rect{220, 98, 24, 24}));
    assert(wc.predict(files[0].first, rect{20, 279, 24, 24}));
}

void test_weak_classifier_sanity_2()
{
    const int size = 100;
    bitmap8 bm(size, size);
    bm.m_data.resize(size*size);

    const auto mid = bm.m_data.begin() + (bm.m_data.size() / 2);
    std::fill(bm.m_data.begin(), mid, 0);
    std::fill(mid, bm.m_data.end(), 255);
    auto ii = integral_image<bitmap8>::create(bm);

    using ii_t = decltype(ii);
    haar_feature<ii_t, 24, 24> feature(std::vector<rect>{ rect{0, 0, 2, 2} },
                                       std::vector<rect>{ rect{2, 0, 2, 2} });
    weak_classifier<ii_t, 24, 24> w {std::move(feature)};

    using data_t = typename weak_classifier<ii_t, 24, 24>::data_t;
    const auto pos = ii.vectorize_window(rect {0, 0, 24, 24});
    data_t posv(10, pos);
    const auto neg = ii.vectorize_window(rect {76, 76, 24, 24});
    data_t negv(10, pos);
    w.train(posv, negv);
    std::cout << w.threshold << ", " << w.parity << "\n";
    w.predict(ii, rect {0, 0, 24, 24});
}


void test_integral_image()
{
    const int size = 100;
    bitmap8 bm(size, size);
    bm.m_data.resize(size*size);
    std::fill(bm.m_data.begin(), bm.m_data.end(), 128);
    auto ii = integral_image<bitmap8>::create(bm);
    assert(ii.at(0, 0)               == 128 * 1);
    assert(ii.at(0, size - 1)        == 128 * size);
    assert(ii.at(size - 1, 0)        == 128 * size);
    assert(ii.at(size - 1, size - 1) == 128 * size * size);
    assert(ii.at(10, 23)             == 128 * 11 * 24);
}

void test_weak_classifier_sanity()
{
    const int size = 100;
    bitmap8 bm(size, size);
    bm.m_data.resize(size*size);
    std::fill(bm.m_data.begin(), bm.m_data.end(), 128);
    auto ii = integral_image<bitmap8>::create(bm);
    std::vector<rect> pos {
            rect {0, 0, 2, 2}
    };
    std::vector<rect> neg {
            rect {2, 2, 2, 2}
    };
    using ii_t = decltype(ii);
    haar_feature<ii_t, 24, 24> feature(std::move(pos), std::move(neg));
    using ii_t = decltype(ii);
    weak_classifier<ii_t, 24, 24> w;

    w.predict(ii, rect {0, 0, 24, 24});
}

void test_vectorize_subwindow()
{
    const int size = 100;
    bitmap8 bm(size, size);
    bm.m_data.resize(size * size);

    for (auto i = 0; i < bm.m_data.size(); ++i)
    {
        bm.m_data[i] = i;
    }

    auto ii = integral_image<bitmap8>::create(bm);
    rect window{10, 10, 10, 10};
    auto vectorized_window = ii.vectorize_window(window);
    auto it = vectorized_window.begin();
    for (auto y = 10; y < 20; ++y)
    {
        for (auto x = 10; x < 20; ++x)
        {
            assert(*it == ii.at(x, y));
            ++it;
        }
    }
}

void test_read_bmp()
{
    std::ifstream in("test.bmp");
    bitmap24 bm = from_file<bitmap24>(in);
    // This test assumes that 24-bit pixels are stored in BGR-order instead of RGB-order.
    assert(bm.at(0, 0) == std::make_tuple(0, 0, 255));
    assert(bm.at(1, 0) == std::make_tuple(0, 255, 0));
    assert(bm.at(2, 0) == std::make_tuple(255, 0, 0));
    assert(bm.at(3, 0) == std::make_tuple(255, 255, 255));
    assert(bm.at(7, 7) == std::make_tuple(0, 0, 0));
}


void test_create_bmp24()
{
    std::ifstream in("test.bmp");
    bitmap24 bm = from_file<bitmap24>(in);
    auto ii = integral_image<bitmap24>::create(bm);
    assert(ii.at(0, 0) == bm.at(0, 0));
}


int main()
{
    MCG_RUN_TEST(test_integral_image);
    MCG_RUN_TEST(test_weak_classifier_sanity);
    MCG_RUN_TEST(test_vectorize_subwindow);
    MCG_RUN_TEST(test_read_bmp);
    MCG_RUN_TEST(test_create_bmp24);
    MCG_RUN_TEST(test_weak_classifier_sanity_2);
    MCG_RUN_TEST(test_weak_classifier_with_bioid_dataset);
    return 0;
}