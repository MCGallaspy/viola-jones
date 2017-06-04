//
// Created by Mike on 6/3/2017.
//
#ifndef VIOLA_JONES_GENERATE_WEAK_CLASSIFIERS_H
#define VIOLA_JONES_GENERATE_WEAK_CLASSIFIERS_H
#include "weak_classifier.h"
#include <vector>

namespace mcg
{

namespace haar_feature_kinds
{
/*
 * A haar-like feature are adjacent rectangles with alternating weights.
 * The rectangles' dimensions are constrained to form a larger rectangle.
 *
 * The kind of the feature is totally specified by how the larger rectangle is subdivided.
 * The word "kind" is used to distinguish from the C++ term "type".
 * We count the number of times the larger rectangle is divided on its vertical and horizontal axes.
 */

struct kind_1
{
    static constexpr int horizontal_divisions = 1;
    static constexpr int vertical_divisions = 0;
};

} // namespace mcg::haar_feature_kinds

/*
 * Declarations
 */

template <typename weak_classifier_t, typename feature_t, typename output_iterator_t>
struct generate_features
{
    static void run(output_iterator_t out);
};

namespace detail_gwc
{
template <size_t n>
void next_limits(std::array<size_t, n> &arr, size_t max);
} // namespace detail_gwc

/*
 * Definitions
 */

template<size_t n>
void detail_gwc::next_limits(std::array<size_t, n> &arr, size_t max)
{
    for (auto i = 0; i < arr.size(); ++i)
    {
        const auto cur = arr.size() - 1 - i;
        auto& e = arr[cur];
        ++e;
        for (auto j = cur + 1; j < arr.size(); ++j)
        {
            arr[j] = arr[j - 1] + 1;
        }
        if (e <= (max - i))
        {
            break;
        }
    }
    if (arr[0] > max - arr.size() + 1)
    {
        for (auto i = 0; i < arr.size(); ++i)
        {
            arr[i] = i;
        }
    }
}

template <typename feature_kind, typename output_iterator_t, typename integral_image_t, size_t width, size_t height>
struct generate_features<
        weak_classifier<integral_image_t, width, height>,
        feature_kind,
        output_iterator_t>
{
    static void run(output_iterator_t out)
    {
        static constexpr size_t min_width = feature_kind::horizontal_divisions + 1;
        static constexpr size_t min_height = feature_kind::vertical_divisions + 1;
        // The "limits" are an ordered tuple, where the first element is where the large rectangle starts,
        // the following elements indicate where the subdivisions start,
        // and the final element is one past where the last subdivision ends.
        std::array<size_t, feature_kind::horizontal_divisions + 2> h_limits;
        for (auto i = 0; i < h_limits.size(); ++i)
        {
            h_limits[i] = i;
        }
        std::array<size_t, feature_kind::vertical_divisions + 2> v_limits;
        for (auto i = 0; i < v_limits.size(); ++i)
        {
        v_limits[i] = i;
        }
        const auto h_begin = h_limits;
        const auto v_begin = v_limits;
        do {
            haar_feature<integral_image_t, width, height> feature;
            std::vector<rect> pos, neg;
            detail_gwc::next_limits(h_limits, width);
            detail_gwc::next_limits(v_limits, height);
        } while (
                std::lexicographical_compare(h_begin.begin(), h_begin.end(), h_limits.begin(), h_limits.end())
                || std::lexicographical_compare(v_begin.begin(), v_begin.end(), v_limits.begin(), v_limits.end())
                );
    }
};


using wc_t = weak_classifier<integral_image<bitmap8>, 24, 24>;
using myvec = std::vector<wc_t>;
template <typename iterator_t>
using test = generate_features<wc_t, haar_feature_kinds::kind_1, iterator_t>;

static void testfoo()
{
    myvec foo;
    test<decltype(std::back_inserter(foo))>::run(std::back_inserter(foo));
}

} // namespace mcg

#endif //VIOLA_JONES_GENERATE_WEAK_CLASSIFIERS_H
