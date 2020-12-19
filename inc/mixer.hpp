/******************************************************************************
Copyright 2020 Taras Zakharchenko taras.zakharchenko@gmail.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1.  Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
3.  Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
#ifndef _MIXER_HPP_
#define _MIXER_HPP_

#include "error.hpp"
#include <cstdlib>
#include <vector>

namespace DSP
{
    /*! RF mixer class */
    template <class T> class Mixer
    {
        public:
            /** @brief Constructor to Mixer class
             *  @param[in] fs sampling rate
             *  @param[in] f frequency to mix with
             *  @param[in] initial_length how many samples of 'f' to presynthesize
             */
            Mixer(double fs, double f, size_t initial_length);
            /** @brief mix method of Mixer class
             *  @param [in] input_i input I-data
             *  @param [in] input_q input Q-data
             *  @param [out] output_i output I-data
             *  @param [out] output_q output Q-data
             *  @param [in] len length of the data
             *  @return Error code. SUCCESS if no problems.
             */
            Error mix(const T *input_i, const T *input_q, T *output_i, T *output_q, size_t len);
        private:
            Error resizeFrame(size_t initial_length);
            double fs;
            double f;
            T step;
            std::vector<T> sine_i;
            std::vector<T> sine_q;
    };
    Error mix(double fs, double f, const double *input_i, const double *input_q, double *output_i, double *output_q, size_t len);
    Error mix(float fs, float f, const float *input_i, const float *input_q, float *output_i, float *output_q, size_t len);
}

#endif /*_MIXER_HPP_*/