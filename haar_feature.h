//
// Created by Mike on 5/18/2017.
//

#ifndef VIOLA_JONES_HAAR_FEATURE_H
#define VIOLA_JONES_HAAR_FEATURE_H
#include "integral_image.h"
#include "rect.h"
#include <cstdint>

namespace mcg
{

/*
 * A Haar-like feature is defined by two sequences of rectangles that
 * are its positive and negative regions. This
 */
class haar_feature
{
public:
    haar_feature(std::vector<rect>&& pos, std::vector<rect>&& neg);

    template <typename integral_image_t>
    typename integral_image_t::sum_t
    evaluate_at(const integral_image_t& ii, size_t x_offset, size_t y_offset);

private:
    std::vector<rect> m_pos, m_neg;
};

} // namespace mcg

#endif //VIOLA_JONES_HAAR_FEATURE_H
