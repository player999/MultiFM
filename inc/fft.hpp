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
#ifndef _FFT_HPP_
#define _FFT_HPP_
#include <cstdint>
#include <cstdlib>
#include <vector>
#include "error.hpp"

namespace DSP
{
    /*! Fast fourier transform class */
    template <class T> class FFT
    {
        public:
            /** Constructor for FFT class */
            FFT();
            /** @brief Do FFT for 2^n signal size.
             *  @param [in] input_i input I-data
             *  @param [in] input_q input q-data
             *  @param [out] output_i output I-data
             *  @param [out] output_q output q-data
             *  @param [in] length of the signal
             *  @return Returns #SUCCESS on success.
             */
            Error transform2(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            /** @brief Do inverse FFT for 2^n signal size.
             *  @param [in] input_i input I-data
             *  @param [in] input_q input q-data
             *  @param [out] output_i output I-data
             *  @param [out] output_q output q-data
             *  @param [in] length of the signal
             *  @return Returns #SUCCESS on success.
             */
            Error inverse_transform2(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            Error transform_bluestein(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            Error inverse_transform_bluestein(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            Error transform(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            Error inverse_transform(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
        private:
            Error updateW(size_t s, bool forward);
            void updateBitrev(size_t s);
            size_t singleBitrev(size_t idx, uint8_t w);
            std::vector<T> wi;
            std::vector<T> wq;
            std::vector<T> inverse_wi;
            std::vector<T> inverse_wq;
            std::vector<size_t> bitrev;
    };
}

#endif /*_FFT_HPP_*/
