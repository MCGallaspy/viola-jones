cmake_minimum_required(VERSION 3.7)
project(viola_jones)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        bitmap.cpp
        bitmap.h
        integral_image.cpp
        integral_image.h)

add_library(vj_mcg STATIC ${SOURCE_FILES})