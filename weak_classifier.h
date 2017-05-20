//
// Created by Mike on 5/18/2017.
//

#ifndef VIOLA_JONES_WEAK_CLASSIFIER_H
#define VIOLA_JONES_WEAK_CLASSIFIER_H
#include "haar_feature.h"
#include "integral_image.h"
#include "rect.h"

namespace mcg {

struct weak_classifier
{
    using sum_t = integral_image::sum_t;
    bool parity;
    sum_t threshold;
    haar_feature feature;

    bool predict(const integral_image& ii, const rect& subwindow);

    using data_t = std::vector<const integral_image&>;
    void train(const data_t& data);
};

} // namespace mcg

#endif //VIOLA_JONES_WEAK_CLASSIFIER_H
