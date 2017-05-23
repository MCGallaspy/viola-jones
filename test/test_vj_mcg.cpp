//
// Created by Mike on 5/17/2017.
//
#include "basic_bitmap.h"
#include "integral_image.h"
#include "weak_classifier.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <fstream>

using namespace mcg;

template <typename func_t>
void test_runner(std::string&& m_name, func_t&& m_func)
{
    std::cout << "Running " << m_name << "... ";
    m_func();
    std::cout << "finished " << m_name << ".\n";
}

#define MCG_RUN_TEST(test_name) \
    test_runner(#test_name, test_name);

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
    return 0;
}