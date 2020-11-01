cmake_minimum_required(VERSION 3.18)
project(FmDemod VERSION 1.0.0 DESCRIPTION "FM Radio demodulation")

add_library(cutedsp SHARED
    filter.cpp
)
add_executable(filter_test cutedsp_test.cpp)

target_include_directories(cutedsp PRIVATE .)
target_include_directories(filter_test PRIVATE .)
target_link_libraries(filter_test PUBLIC cutedsp gtest)