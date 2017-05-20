//
// Created by Mike on 5/17/2017.
//

#ifndef VIOLA_JONES_INTEGRAL_IMAGE_H
#define VIOLA_JONES_INTEGRAL_IMAGE_H
#include "bitmap.h"
#include "rect.h"
#include <vector>

namespace mcg
{

class integral_image
{
public:
    using sum_t = std::uint_fast32_t;
    using size_type = bitmap::size_type;

    explicit integral_image(const bitmap& bm);

    const sum_t& at(size_type x, size_type y) const;
    sum_t& at(size_type x, size_type y);

    std::vector<sum_t> vectorize_window(const rect &subwindow);

private:
    size_type m_width, m_height;
    std::vector<sum_t> m_data;
};

} // namespace mcg
#endif //VIOLA_JONES_INTEGRAL_IMAGE_H
