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
    bool parity;
    double threshold;
    haar_feature feature;

    bool predict(const integral_image& ii, const rect& subwindow);
};

} // namespace mcg

#endif //VIOLA_JONES_WEAK_CLASSIFIER_H
