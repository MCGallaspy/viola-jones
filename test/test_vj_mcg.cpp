//
// Created by Mike on 5/17/2017.
//
#include "bitmap.h"
#include "integral_image.h"

#include <algorithm>
#include <iostream>

using namespace mcg;

int main()
{
    const int size = 100;
    bitmap bm(size, size);
    bm.m_data.resize(size*size);
    std::fill(bm.m_data.begin(), bm.m_data.end(), 128);
    integral_image ii(bm);
    std::cout << ii.at(0, 0) << "\n"
              << ii.at(0, size - 1) << "\n"
              << ii.at(size - 1, 0) << "\n"
              << ii.at(size - 1, size - 1);
    return 0;
}