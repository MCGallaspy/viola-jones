//
// Created by Mike on 5/17/2017.
//
#ifndef VIOLA_JONES_BITMAP_H
#define VIOLA_JONES_BITMAP_H
#include <cassert>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>

namespace mcg
{

template <typename _pixel_t>
struct basic_bitmap
{
    using size_type = std::size_t;
    using pixel_t = _pixel_t;

    basic_bitmap(size_type width, size_type height);

    const pixel_t& at(size_type width, size_type height) const;
    pixel_t& at(size_type width, size_type height);

    const size_type m_width;
    const size_type m_height;
    std::vector<pixel_t> m_data;
};

template <typename bitmap_t>
bitmap_t from_file(std::istream& in);

/**************
* Definitions *
**************/
template <typename pixel_t>
basic_bitmap<pixel_t>::basic_bitmap(size_type width, size_type height)
        : m_width(width)
        , m_height(height)
        , m_data()
{
    m_data.reserve(width*height);
}

template <typename pixel_t>
const pixel_t& basic_bitmap<pixel_t>::at(size_type width, size_type height) const
{
    return m_data[height * m_width + width];
}

template <typename pixel_t>
pixel_t& basic_bitmap<pixel_t>::at(size_type width, size_type height)
{
    return const_cast<pixel_t&>(static_cast<const basic_bitmap<pixel_t>*>(this)->at(width, height));
}

template <typename ret_t>
static ret_t from_le_byte_array(const uint8_t* in)
{
    ret_t mul = 1;
    ret_t size = 0;
    for (int i = 0; i < sizeof(ret_t); ++i)
    {
        size += static_cast<ret_t>(in[i]) * mul;
        mul *= 256;
    }
    return size;
}

template <typename bitmap_t>
bitmap_t from_file(std::istream& in)
{
    struct {
        struct {
            int8_t bm[2] = {'B', 'M'};
            uint8_t filesize[4];
            int8_t reserved[4];
            uint8_t offset[4];
        } bmp_header;
        uint8_t dib_header_size[4];
    } file_header;

    struct {
        uint8_t width[4],
                height[4],
                color_planes[2],
                bits_per_pixel[2],
                compression_method[4],
                image_size[4],
                horizontal_resolution[4],
                vertical_resolution[4],
                color_palette_size[4],
                important_colors[4];
    } dib_header;

    in.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));

    assert(file_header.bmp_header.bm[0] == 'B');
    assert(file_header.bmp_header.bm[1] == 'M');
    assert(from_le_byte_array<uint32_t>(file_header.dib_header_size) - 4 == sizeof(dib_header));

    const auto size = from_le_byte_array<uint32_t>(file_header.bmp_header.filesize);
    std::cout << "Filesize " << size << ". ";
    std::cout << "DIB header size " << from_le_byte_array<uint32_t>(file_header.dib_header_size) << ". ";
    in.read(reinterpret_cast<char*>(&dib_header), sizeof(dib_header));

    std::cout << "Image size is "
              << from_le_byte_array<int32_t>(dib_header.width) << "x"
              << from_le_byte_array<int32_t>(dib_header.height) << ". ";
    std::cout << "Color planes " << from_le_byte_array<uint16_t>(dib_header.color_planes) << ". ";
    std::cout << "Bits per pixel " << from_le_byte_array<uint16_t>(dib_header.bits_per_pixel) << ". ";

    // Skip reading the other info for now and go directly to the pixel array
    // Note in general that the info in the rest of the header must be considered in order to properly
    // interpret the data, e.g. the number of bits per pixel or the data in the color table.
    in.seekg(from_le_byte_array<uint32_t>(file_header.bmp_header.offset));

    assert(from_le_byte_array<uint16_t>(dib_header.bits_per_pixel) == 24);
    const int32_t width = from_le_byte_array<int32_t>(dib_header.width);
    const int32_t height = from_le_byte_array<int32_t>(dib_header.height);
    bitmap_t bm(width, height);
    for (auto i = 0; i < width * height; ++i)
    {
        typename bitmap_t::pixel_t p;
        in >> p;
        bm.m_data.push_back(p);
    }
    return bm;
}

using bitmap8 = basic_bitmap<uint8_t>;
using bitmap24 = basic_bitmap<std::tuple<uint8_t>>;

} // namespace mcg

#endif //VIOLA_JONES_BITMAP_H
