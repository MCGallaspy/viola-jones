//
// Created by Mike on 5/18/2017.
//

#include "weak_classifier.h"

using namespace mcg;

bool weak_classifier::predict(const integral_image &ii, const rect &subwindow) {
    return false;
}

void weak_classifier::train(const weak_classifier::data_t &data) {
    // We can just search the whole parameter space.
}
