//
// Created by Mike on 5/17/2017.
//
#ifndef VIOLA_JONES_BITMAP_H
#define VIOLA_JONES_BITMAP_H
#include <vector>
#include <cstdint>

namespace mcg
{

// A thin-wrapper around a row-major monochrom bitmap image
struct bitmap
{
    using size_type = std::size_t;
    using pixel_t = std::uint8_t;

    bitmap(size_type width, size_type height);

    const pixel_t& at(size_type width, size_type height) const;
    pixel_t& at(size_type width, size_type height);

    const size_type m_width;
    const size_type m_height;
    std::vector<pixel_t> m_data;
};

} // namespace mcg

#endif //VIOLA_JONES_BITMAP_H
