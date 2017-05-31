//
// Created by Mike on 5/18/2017.
//

#ifndef VIOLA_JONES_WEAK_CLASSIFIER_H
#define VIOLA_JONES_WEAK_CLASSIFIER_H
#include "haar_feature.h"
#include "integral_image.h"
#include "rect.h"
#include <limits>

namespace mcg {

template <typename integral_image_t, size_t _width, size_t _height>
struct weak_classifier
{
    static constexpr double MIN_TRUE_POS_RATE = 0.5;
    using sum_t = typename integral_image_t::sum_t;
    using haar_feature_t = haar_feature<integral_image_t, _width, _height>;

    // data
    haar_feature_t feature;
    bool parity;
    sum_t threshold;

    bool predict(const integral_image_t& ii, const rect& subwindow);

    using data_t = std::vector<std::vector<sum_t>>;
    void train(const weak_classifier::data_t &positive,
                   const weak_classifier::data_t &negative);
};

template <typename ii_t, size_t width, size_t height>
bool
weak_classifier<ii_t, width, height>::predict(const ii_t &ii, const rect &subwindow)
{
    const auto window = ii.vectorize_window(subwindow);
    const auto val = feature.evaluate(window);
    if (parity)
    {
        return val < threshold;
    } else {
        return val > threshold;
    }
}

template <typename ii_t, size_t width, size_t height>
void
weak_classifier<ii_t, width, height>::train(const weak_classifier::data_t &positive,
                                            const weak_classifier::data_t &negative)
{
    threshold = 0;
    parity = true;
    std::vector<sum_t> ps, ns;
    std::transform(positive.begin(), positive.end(), std::back_inserter(ps), [this](const typename data_t::value_type& p){
        return this->feature.evaluate(p);
    });
    std::transform(negative.begin(), negative.end(), std::back_inserter(ns), [this](const typename data_t::value_type& p){
        return this->feature.evaluate(p);
    });
    std::sort(ps.begin(), ps.end());
    std::sort(ns.begin(), ns.end());
    auto minmax_res = std::minmax_element(ps.begin(), ps.end());
    const auto min = minmax_res.first;
    const auto max = minmax_res.second;
    auto min_false_pos_count = std::count_if(ns.begin(), ns.end(), [min](sum_t e) {
        return e < *min;
    });
    auto max_false_pos_count = std::count_if(ns.begin(), ns.end(), [max](sum_t e) {
        return e > *max;
    });
    double false_pos_rate;
    parity = min_false_pos_count < max_false_pos_count;
    if (parity)
    {
        false_pos_rate = static_cast<double>(max_false_pos_count) / ns.size();
        threshold = *max;
    }
    else
    {
        false_pos_rate = static_cast<double>(min_false_pos_count) / ns.size();
        threshold = *min;
    }
}

} // namespace mcg

#endif //VIOLA_JONES_WEAK_CLASSIFIER_H
