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
    integral_image ii(bm);
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
    integral_image ii(bm);
    std::vector<rect> pos {
            rect {0, 0, 2, 2}
    };
    std::vector<rect> neg {
            rect {2, 2, 2, 2}
    };
    haar_feature feature(std::move(pos), std::move(neg));
    using threshold_t = decltype(std::declval<weak_classifier>().threshold);
    const auto threshold = std::numeric_limits<threshold_t>::max() / 2;
    weak_classifier w{true, threshold, std::move(feature)};

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

    integral_image ii(bm);
    rect window{10, 10, 10, 10};
    auto vectorized_window = ii.vectorize_window(window);
    auto it = vectorized_window.begin();
    for (auto x = 10; x < 20; ++x)
    {
        for (auto y = 10; y < 20; ++y)
        {
            assert(*it = ii.at(x, y));
            ++it;
        }
    }
}

void test_read_bmp()
{
    std::ifstream in("test.bmp");
    bitmap8 bm = from_file<bitmap8>(in);
    for (auto j = 0; j < bm.m_height; ++j)
    {
        for (auto i = 0; i < bm.m_width; ++i)
        {
            std::cout << static_cast<int32_t>(bm.at(i, j)) << " ";
        }
        std::cout << "\n";
    }
}

int main()
{
    MCG_RUN_TEST(test_integral_image);
    MCG_RUN_TEST(test_weak_classifier_sanity);
    MCG_RUN_TEST(test_vectorize_subwindow);
    MCG_RUN_TEST(test_read_bmp);
    return 0;
}