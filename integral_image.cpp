//
// Created by Mike on 5/17/2017.
//
#include "bitmap.h"
#include "integral_image.h"

#include <vector>
#include <cassert>
#include <iostream>

using namespace mcg;
using sum_t = integral_image::sum_t;

integral_image::integral_image(const bitmap& bm)
    : m_width(bm.m_width)
    , m_height(bm.m_height)
    , m_data(m_width * m_height, 0)
{
    std::vector<sum_t> row(m_width, 0);
    for (size_type i = 0; i < m_height; ++i) {
        for (size_type j = 0; j < m_width; ++j) {
            if (j > 0) {
                row[j] = row[j - 1] + bm.at(j, i);
            } else {
                row[j] = bm.at(j, i);
            }
            if (i > 0) {
                at(j, i) = at(j, i - 1) + row[j];
            } else {
                at(j, i) = row[j];
            }
        }
    }
}

const sum_t& integral_image::at(size_type x, size_type y) const
{
    return m_data[y * m_width + x];
}

sum_t& integral_image::at(size_type x, size_type y)
{
    return const_cast<sum_t&>(static_cast<const integral_image*>(this)->at(x, y));
}

// Copies the given subwindow into contiguous memory
std::vector<sum_t> integral_image::vectorize_window(const rect &subwindow) {
    auto subw = std::vector<sum_t>();
    subw.reserve(subwindow.dx * subwindow.dy);
    for (auto y = subwindow.y; y < subwindow.y + subwindow.dy; ++y)
    {
        std::copy(
                &at(subwindow.x, y), &at(subwindow.x + subwindow.dx, y), // <-- from
                std::back_inserter(subw)                                 // <-- to
        );
    }
    const bool precisely_filled = subw.size() == subw.capacity();
    assert(precisely_filled);
    return subw;
}
