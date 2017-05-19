//
// Created by Mike on 5/17/2017.
//
#include "bitmap.h"
#include "integral_image.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

using namespace mcg;

template <typename func_t>
void test_runner(std::string&& m_name, func_t&& m_func)
{
    std::cout << "Running " << m_name << "...\n";
    m_func();
    std::cout << "Finished " << m_name << ".\n";
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

int main()
{
    MCG_RUN_TEST(test_integral_image);
    return 0;
}