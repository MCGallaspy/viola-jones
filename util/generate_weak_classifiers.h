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

struct kind_2
{
    static constexpr int horizontal_divisions = 0;
    static constexpr int vertical_divisions = 1;
};

struct kind_3
{
    static constexpr int horizontal_divisions = 2;
    static constexpr int vertical_divisions = 0;
};

struct kind_4
{
    static constexpr int horizontal_divisions = 0;
    static constexpr int vertical_divisions = 2;
};

struct kind_5
{
    static constexpr int horizontal_divisions = 1;
    static constexpr int vertical_divisions = 1;
};

using all_kinds = std::tuple<kind_1, kind_2, kind_3, kind_4, kind_5>;

} // namespace mcg::haar_feature_kinds

/*
 * Declarations
 */

template <typename weak_classifier_t, typename feature_t, typename output_iterator_t>
struct generate_features_
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

// This generates strictly more haar-like features than described in Viola-Jones, as here
// we consider subrectangles that are not symmetric.
// If it turns out that considering too many features is computationally prohibitive,
// this should be modified to create fewer
template <typename feature_kind, typename output_iterator_t, typename integral_image_t, size_t width, size_t height>
struct generate_features_<
        weak_classifier<integral_image_t, width, height>,
        feature_kind,
        output_iterator_t>
{
    static void run(output_iterator_t out)
    {
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
        // If switch_row_parities is true, then the next row will have the swapped parity as the previous one.
        // E.g. P - N on the first row (one horizontal division) means the next row should have N - P parity,
        // so that flattened the parities look like P - N - N - P.
        // whereas P - N - P (two horizontal divisions) should continue the switching and get N - P - N, which
        // when flattened looks like P - N - P - N - P - N.
        const bool switch_row_parities = feature_kind::horizontal_divisions % 2 == 1;
        const auto h_begin = h_limits;
        const auto v_begin = v_limits;
        do {
            std::vector<rect> pos, neg;
            bool parity = true;
            for (auto j = 0; j < v_limits.size() - 1; ++j)
            {
                for (auto i = 0; i < h_limits.size() - 1; ++i)
                {
                    const auto x = h_limits[i];
                    const auto y = v_limits[j];
                    const auto dx = h_limits[i + 1] - x;
                    const auto dy = v_limits[j + 1] - y;
                    rect r{x, y, dx, dy};
                    if (parity)
                    {
                        pos.push_back(std::move(r));
                    }
                    else
                    {
                        neg.push_back(std::move(r));
                    }
                    parity = !parity;
                }
                parity = switch_row_parities ? !parity : parity;
            }
            haar_feature<integral_image_t, width, height> feature(std::move(pos), std::move(neg));
            *out = weak_classifier<integral_image_t, width, height>{std::move(feature)};
            detail_gwc::next_limits(h_limits, width);
            const bool step_vertical =
                    !std::lexicographical_compare(h_begin.begin(), h_begin.end(), h_limits.begin(), h_limits.end());
            if (step_vertical)
            {
                detail_gwc::next_limits(v_limits, height);
            }
        } while (
                std::lexicographical_compare(h_begin.begin(), h_begin.end(), h_limits.begin(), h_limits.end())
                || std::lexicographical_compare(v_begin.begin(), v_begin.end(), v_limits.begin(), v_limits.end())
                );
    }
};

} // namespace mcg

#endif //VIOLA_JONES_GENERATE_WEAK_CLASSIFIERS_H
