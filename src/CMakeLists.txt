cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=znver2")

add_library(cutedsp SHARED filter.cpp mixer.cpp atan.cpp diff.cpp audio_encoder.cpp interleaving.cpp)
target_link_libraries(cutedsp PUBLIC avcodec)
target_include_directories(cutedsp PRIVATE ../inc)

add_executable(filter_test cutedsp_test.cpp)
target_include_directories(filter_test PRIVATE ../inc)
target_link_libraries(filter_test PUBLIC cutedsp gtest)

add_executable(multifm_console_app multifm_console_app.cpp)
target_include_directories(multifm_console_app PRIVATE ../inc)
target_link_libraries(multifm_console_app PUBLIC cutedsp boost_program_options pthread)