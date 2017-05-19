//
// Created by Mike on 5/19/2017.
//

#ifndef VIOLA_JONES_RECT_H
#define VIOLA_JONES_RECT_H
#include <cstdint>

namespace mcg {

/*
 * A rectangle, where (x, y) represents the top left point,
 * and dx and dy represent the extent in the x and y directions.
 *
 * A pixel (X, Y) in the feature satisfies:
 *      x <= X < x + dx
 *      y <= Y < y + dy
 *
 * (Right is the positive x direction and down is the positive y
 * direction).
 */
struct rect {
    std::size_t x, y, dx, dy;
};

} // namespace mcg

#endif //VIOLA_JONES_RECT_H
