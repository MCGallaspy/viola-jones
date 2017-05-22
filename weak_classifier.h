//
// Created by Mike on 5/18/2017.
//

#ifndef VIOLA_JONES_WEAK_CLASSIFIER_H
#define VIOLA_JONES_WEAK_CLASSIFIER_H
#include "haar_feature.h"
#include "integral_image.h"
#include "rect.h"

namespace mcg {

template <typename integral_image_t>
struct weak_classifier
{
    using sum_t = typename integral_image_t::sum_t;
    bool parity;
    sum_t threshold;
    haar_feature feature;

    bool predict(const integral_image_t& ii, const rect& subwindow);

    using data_t = std::vector<const integral_image_t&>;
    void train(const data_t& data);
};

template <typename ii_t>
bool weak_classifier<ii_t>::predict(const ii_t &ii, const rect &subwindow) {
    return false;
}

template <typename ii_t>
void weak_classifier<ii_t>::train(const weak_classifier::data_t &data) {

}

} // namespace mcg

#endif //VIOLA_JONES_WEAK_CLASSIFIER_H
