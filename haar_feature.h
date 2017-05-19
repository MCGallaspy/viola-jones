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
    using sum_t = integral_image::sum_t;
    haar_feature(std::vector<rect>&& pos, std::vector<rect>&& neg);
    sum_t evaluate_at(const integral_image& ii, size_t x_offset, size_t y_offset);

private:
    std::vector<rect> m_pos, m_neg;
};

} // namespace mcg

#endif //VIOLA_JONES_HAAR_FEATURE_H
