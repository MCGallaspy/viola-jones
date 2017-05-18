//
// Created by Mike on 5/17/2017.
//

#include "integral_image.h"
using namespace mcg;
using sum_t = integral_image::sum_t;

const sum_t& integral_image::at(size_type width, size_type height) const
{
    return m_data[height * m_width + width];
}

sum_t& integral_image::at(size_type width, size_type height)
{
    return const_cast<sum_t&>(static_cast<const integral_image*>(this)->at(width, height));
}