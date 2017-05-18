//
// Created by Mike on 5/17/2017.
//

#include "bitmap.h"
#include <vector>

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
