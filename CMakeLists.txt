cmake_minimum_required(VERSION 3.7)
project(viola_jones)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        basic_bitmap.h
        integral_image.h haar_feature.h weak_classifier.h util/generate_weak_classifiers.h)

include_directories(.)

add_library(vj_mcg STATIC ${SOURCE_FILES})

add_executable(test_vj_mcg ${SOURCE_FILES} test/test_vj_mcg.cpp)