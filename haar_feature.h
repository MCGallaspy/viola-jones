//
// Created by Mike on 5/18/2017.
//

#ifndef VIOLA_JONES_HAAR_FEATURE_H
#define VIOLA_JONES_HAAR_FEATURE_H
#include "integral_image.h"
#include "rect.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>
#include <ostream>

namespace mcg
{
/*
 * Declarations
 */
template <typename _integral_image_t, size_t _width, size_t _height>
class haar_feature;

template <typename _integral_image_t, size_t _width, size_t _height>
std::ostream& operator<<(std::ostream& out, const haar_feature<_integral_image_t, _width, _height>& feature);

/*
 * Definitions
 */

/*
 * A Haar-like feature is defined by two sequences of rectangles that
 * are its positive and negative regions. This
 */
template <typename _integral_image_t, size_t _width, size_t _height>
class haar_feature
{
public:
    static constexpr size_t width = _width;
    static constexpr size_t height = _height;
    using integral_image_t = _integral_image_t;
    using sum_t = typename integral_image_t::sum_t;

    haar_feature(std::vector<rect>&& pos, std::vector<rect>&& neg);

    haar_feature() = default;
    haar_feature(haar_feature&) = default;
    haar_feature& operator=(haar_feature&) = default;
    haar_feature(haar_feature&&) = default;
    haar_feature& operator=(haar_feature&&) = default;

    // The integral image must have the same dimensions as the haar_feature.
    sum_t evaluate(const std::vector<sum_t>& data);

    template <typename _integral_image_t_, size_t _width_, size_t _height_>
    friend std::ostream& operator<<(std::ostream& out, const haar_feature<_integral_image_t_, _width_, _height_>& feature);

private:
    static constexpr size_t length = width * height;
    std::array<sum_t, length> m_data;
};

template <typename _integral_image_t, size_t _width, size_t _height>
std::ostream& operator<<(std::ostream& out, const haar_feature<_integral_image_t, _width, _height>& feature)
{
    using feature_t = haar_feature<_integral_image_t, _width, _height>;
    for (auto j = 0; j < feature_t::height; ++j)
    {
        for (auto i = 0; i < feature_t::width; ++i)
        {
            char c = '.';
            const auto n = j * feature_t::width + i;
            if (feature.m_data[n] == 1) c = 'P';
            if (feature.m_data[n] == -1) c = 'N';
            out << c;
        }
        out << "\n";
    }
    return out;
}

template <typename ii_t, size_t width, size_t height>
typename haar_feature<ii_t, width, height>::sum_t
haar_feature<ii_t, width, height>::evaluate(const std::vector<sum_t>& data)
{
    sum_t sum = 0;
    for (auto i = 0; i < m_data.size(); ++i)
    {
        sum += m_data[i] * data[i];
    }
    return sum;
}

template <typename ii_t, size_t width, size_t height>
haar_feature<ii_t, width, height>::haar_feature(std::vector<rect> &&pos, std::vector<rect> &&neg)
{
    std::fill(m_data.begin(), m_data.end(), 0);
    for (const auto &r : pos)
    {
        for (auto y = r.y; y < r.y + r.dy; ++y)
        {
            for (auto x = r.x; x < r.x + r.dx; ++x)
            {
                m_data[y * width + x] = 1;
            }
        }
    }
    for (const auto &r : neg)
    {
        for (auto y = r.y; y < r.y + r.dy; ++y)
        {
            for (auto x = r.x; x < r.x + r.dx; ++x)
            {
                m_data[y * width + x] = -1;
            }
        }
    }
};

} // namespace mcg

#endif //VIOLA_JONES_HAAR_FEATURE_H
