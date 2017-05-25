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
    static constexpr double MIN_TRUE_POS_RATE = 0.99999;
    using sum_t = typename integral_image_t::sum_t;
    using haar_feature_t = haar_feature<integral_image_t, _width, _height>;

    // data
    haar_feature_t feature;
    bool parity;
    sum_t threshold;

    bool predict(const integral_image_t& ii, const rect& subwindow);

    using data_t = std::vector<std::vector<sum_t>>;
    void train(const weak_classifier::data_t &positive,
                   const weak_classifier::data_t &negative,
                   double max_false_positive_rate);
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
                                            const weak_classifier::data_t &negative,
                                            double max_false_positive_rate)
{
    static constexpr auto MIN = std::numeric_limits<sum_t>::min();
    static constexpr auto MAX = std::numeric_limits<sum_t>::max();
    threshold = 0;
    parity = true;
    double best_false_pos = 1.;
    std::vector<sum_t> ps, ns;
    std::transform(positive.begin(), positive.end(), std::back_inserter(ps), [this](const typename data_t::value_type& p){
        return this->feature.evaluate(p);
    });
    std::transform(negative.begin(), negative.end(), std::back_inserter(ns), [this](const typename data_t::value_type& p){
        return this->feature.evaluate(p);
    });
    for (sum_t cur = 0; cur <= MAX; ++cur)
    {
        uint32_t post{0}, negt{0},
                 posf{0}, negf{0};
        for (const auto& val : ps)
        {
            if (val < cur)
            {
                ++post;
            }
            if (val > cur)
            {
                ++posf;
            }
        }
        for (const auto& val : ns)
        {
            if (val < cur)
            {
                ++negt;
            }
            if (val > cur)
            {
                ++negf;
            }
        }
        const auto pos_size = positive.size();
        const auto neg_size = negative.size();
        const double successful_post = static_cast<double>(post) / pos_size;
        const double successful_posf = static_cast<double>(posf) / pos_size;
        const double false_post = static_cast<double>(negt) / neg_size;
        const double false_posf = static_cast<double>(negf) / neg_size;
        if ( cur % 10000000 == 0) {
            std::fprintf(stderr, "%d:\n\tSuccesses: %.3f, %.3f\n\tFailures: %.3f, %.3f\n",
                         cur,
                         successful_posf, successful_post,
                         false_posf, false_post
            );
        }
        if (successful_post >= MIN_TRUE_POS_RATE && false_post < best_false_pos)
        {
            threshold = cur;
            best_false_pos = false_post;
            parity = true;
        }
        if (successful_posf >= MIN_TRUE_POS_RATE && false_posf < best_false_pos)
        {
            threshold = cur;
            best_false_pos = false_posf;
            parity = false;
        }
        if (best_false_pos < max_false_positive_rate)
        {
            break;
        }
    }
}

} // namespace mcg

#endif //VIOLA_JONES_WEAK_CLASSIFIER_H
