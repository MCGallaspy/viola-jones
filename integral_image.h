//
// Created by Mike on 5/17/2017.
//

#ifndef VIOLA_JONES_INTEGRAL_IMAGE_H
#define VIOLA_JONES_INTEGRAL_IMAGE_H
#include "basic_bitmap.h"
#include "rect.h"
#include <vector>

namespace mcg
{

class integral_image
{
public:
    using sum_t = std::uint_fast32_t;
    using size_type = std::size_t;

    template <typename bitmap_t>
    explicit integral_image(const bitmap_t& bm);

    const sum_t& at(size_type x, size_type y) const;
    sum_t& at(size_type x, size_type y);

    std::vector<sum_t> vectorize_window(const rect &subwindow);

private:
    size_type m_width, m_height;
    std::vector<sum_t> m_data;
};


template <typename bitmap_t>
integral_image::integral_image(const bitmap_t& bm)
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

} // namespace mcg
#endif //VIOLA_JONES_INTEGRAL_IMAGE_H
