//
// Created by Mike on 5/17/2017.
//

#include "bitmap.h"
#include <istream>
#include <vector>
#include <iostream>

using namespace mcg;
using size_type = bitmap::size_type;
using pixel_t = bitmap::pixel_t;

bitmap::bitmap(size_type width, size_type height)
    : m_width(width)
    , m_height(height)
    , m_data()
{
    m_data.reserve(width*height);
}

const pixel_t& bitmap::at(size_type width, size_type height) const
{
    return m_data[height * m_width + width];
}

pixel_t& bitmap::at(size_type width, size_type height)
{
    return const_cast<pixel_t&>(static_cast<const bitmap*>(this)->at(width, height));
}

uint32_t from_le_byte_array(const uint8_t* in)
{
    uint32_t mul = 1;
    uint32_t size = 0;
    for (int i = 0; i < 4; ++i)
    {
        size += static_cast<uint32_t>(in[i]) * mul;
        mul *= 256;
    }
    return size;
}

bitmap mcg::from_file(std::istream& in)
{
    struct {
        struct {
            char bm[2] = {'B', 'M'};
            uint8_t filesize[4];
            char reserved[4];
            uint8_t offset[4];
        } bmp_header;
    } bmp_file;

    in.read(reinterpret_cast<char*>(&bmp_file), sizeof(bmp_file));

    const auto size = from_le_byte_array(bmp_file.bmp_header.filesize);
    std::cout << "Fileize " << size << ". ";
    bitmap bm(0, 0);
    return bm;
}
