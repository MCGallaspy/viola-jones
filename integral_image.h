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

template <typename _bitmap_t>
class integral_image
{
public:
    using bitmap_t = _bitmap_t;
    using sum_t = typename get_sum_t<bitmap_t>::sum_t;
    using size_type = typename bitmap_t::size_type;

    const sum_t& at(size_type x, size_type y) const;
    sum_t& at(size_type x, size_type y);

    std::vector<sum_t> vectorize_window(const rect &subwindow);

    static integral_image create(const bitmap_t& bm);

private:
    size_type m_width, m_height;
    std::vector<sum_t> m_data;
};

template <typename bitmap_t>
const typename integral_image<bitmap_t>::sum_t&
integral_image<bitmap_t>::at(size_type x, size_type y) const
{
    return m_data[y * m_width + x];
}

template <typename bitmap_t>
typename integral_image<bitmap_t>::sum_t&
integral_image<bitmap_t>::at(size_type x, size_type y)
{
    return const_cast<sum_t&>(static_cast<const integral_image*>(this)->at(x, y));
}

// Copies the given subwindow into contiguous memory
template <typename bitmap_t>
std::vector<typename integral_image<bitmap_t>::sum_t>
integral_image<bitmap_t>::vectorize_window(const rect &subwindow) {
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

template<typename bitmap_t>
integral_image<bitmap_t>
integral_image<bitmap_t>::create(const bitmap_t &bm) {
    using sum_t = integral_image<bitmap8>::sum_t;
    using size_type = integral_image<bitmap8>::size_type;
    using ii_t = integral_image<bitmap8>;
    ii_t ii;
    ii.m_width = bm.m_width;
    ii.m_height = bm.m_height;
    ii.m_data.resize(ii.m_width * ii.m_height);
    std::vector<sum_t> row(ii.m_width, 0);
    for (size_type i = 0; i < ii.m_height; ++i) {
        for (size_type j = 0; j < ii.m_width; ++j) {
            if (j > 0) {
                row[j] = row[j - 1] + bm.at(j, i);
            } else {
                row[j] = bm.at(j, i);
            }
            if (i > 0) {
                ii.at(j, i) = ii.at(j, i - 1) + row[j];
            } else {
                ii.at(j, i) = row[j];
            }
        }
    }
    return ii;
}

} // namespace mcg
#endif //VIOLA_JONES_INTEGRAL_IMAGE_H
