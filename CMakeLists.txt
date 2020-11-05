cmake_minimum_required(VERSION 3.18)
project(FmDemod VERSION 1.0.0 DESCRIPTION "FM Radio demodulation")

set(Matlab_ROOT_DIR "/home/player999/storage/program_files/Matlab")
find_package(Matlab)
matlab_add_mex(NAME filter_ml SRC filter_ml.c LINK_TO cutedsp)

add_library(cutedsp SHARED filter.cpp)
add_executable(filter_test cutedsp_test.cpp)

target_include_directories(cutedsp PRIVATE .)
target_include_directories(filter_test PRIVATE .)
target_link_libraries(filter_test PUBLIC cutedsp gtest)