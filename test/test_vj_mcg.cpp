//
// Created by Mike on 5/17/2017.
//
#include "bitmap.h"
#include "integral_image.h"
#include "weak_classifier.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

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
    bitmap bm(size, size);
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
    bitmap bm(size, size);
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
    weak_classifier w{true, 0.5, std::move(feature)};

    w.predict(ii, rect {0, 0, 24, 24});
}

int main()
{
    MCG_RUN_TEST(test_integral_image);
    MCG_RUN_TEST(test_weak_classifier_sanity);
    return 0;
}