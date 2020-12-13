# Copyright 2020 Taras Zakharchenko taras.zakharchenko@gmail.com

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1.  Redistributions of source code must retain the above copyright notice, this
#     list of conditions and the following disclaimer.
# 2.  Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
# 3.  Neither the name of the copyright holder nor the names of its contributors
#     may be used to endorse or promote products derived from this software
#     without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

add_library(cutedsp SHARED filter.cpp mixer.cpp atan.cpp diff.cpp audio_encoder.cpp interleaving.cpp fft.cpp find_stations.cpp rfsource.cpp fm_receiver.cpp tracer_tong.cpp)
target_link_libraries(cutedsp PUBLIC avcodec hackrf rtlsdr)
target_include_directories(cutedsp PRIVATE ../inc)

add_executable(filter_test cutedsp_test.cpp)
target_include_directories(filter_test PRIVATE ../inc)
target_link_libraries(filter_test PUBLIC cutedsp pthread gtest)

add_executable(multifm_console_app multifm_console_app.cpp)
target_include_directories(multifm_console_app PRIVATE ../inc)
target_link_libraries(multifm_console_app PUBLIC cutedsp boost_program_options pthread)
