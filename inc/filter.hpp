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
#ifndef _FILTER_HPP_
#define _FILTER_HPP_
#include <vector>
#include <tuple>
#include <cstdint>
#include "error.hpp"

namespace DSP
{
    /*! Filter type */
    enum FilterType
    {
        /** Lowpass filter */
        FILTER_TYPE_LP,
        /** Highpass filter */
        FILTER_TYPE_HP,
        /** Bandpass filter */
        FILTER_TYPE_BP,
        /** Custom filtering coefficient */
        FILTER_TYPE_CUSTOM,
    };

    /*! Filter window */
    enum WindowFunction
    {
        /** Bartlett window */
        BARTLETT,
        /** Hanning window */
        HANNING,
        /** Blackman window */
        BLACKMAN,
        /** Hamming window */
        HAMMING,
        /** Blackman-Harris window */
        BLACKMAN_HARRIS,
        /** Blackman-Nuttal window */
        BLACKMAN_NUTTAL,
        /** Nuttal window */
        NUTTAL,
    };

    /*! FIR Filter class */
    template <class T> class FirFilter
    {
        public:
            /** @brief FirFilter constructor, which creates filter from parameters.
             * @param[in] ft filter type
             * @param[in] fs sampling rate
             * @param[in] f1 lower frequency
             * @param[in] f2 upper frequency
             * @param[in] order coefficient count in filter (it's order)
             * */
            FirFilter(FilterType ft, T fs, T f1, T f2, uint32_t order);
            /** @brief FirFilter constructor, which creates filter from parameters.
             * @param[in] cfs array of filter coefficients
             * */
            FirFilter(std::vector<double> &cfs);
            /** @brief Return Window of given type and order
             * @param[in] window_type Window type
             * @param[in] order window size
             * @param[out] window coefficients
             * @return Error code. SUCCESS in case of success.
             * */
            static Error getWindow(WindowFunction window_type, uint32_t order, T *coefficients);
            /** @brief Execute filter for complex data
             * @param[in] input_i I-data
             * @param[in] input_q Q-data
             * @param[out] output_i I-data
             * @param[out] output_q Q-data
             * @param[in] length - data length
             * @return Error code. SUCCESS in case of success.
             * */
            Error executeCpx(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            /** @brief Execute filter for complex data
             * @param[in] input_i I-data
             * @param[in] input_q Q-data
             * @param[out] output_i I-data
             * @param[out] output_q Q-data
             * @param[in] decimation - decimate output data in 1 to 'decimation'.
             * @param[in] length - data length
             * @return Error code. SUCCESS in case of success.
             * */
            Error executeCpxDecim(T *input_i, T *input_q, T *output_i, T *output_q, uint32_t decim, size_t length);
            /** @brief Execute filter for floating point data
             * @param[in] input data
             * @param[out] output data
             * @param[in] length - data length
             * @return Error code. SUCCESS in case of success.
             * */
            Error executeReal(T *input, T *output, size_t length);
            /** @brief Execute filter for floating point data
             * @param[in] input data
             * @param[out] output data
             * @param[in] decimation - decimate output data in 1 to 'decimation'.
             * @param[in] length - data length
             * @return Error code. SUCCESS in case of success.
             * */
            Error executeRealDecim(T *input, T *output, uint32_t decim, size_t length);
            /** Sampling rate */
            T sampling_rate;
            /** Lower and upper frequencies */
            T f1, f2;
            /** Filtering window */
            std::vector<T> window;
            /** Filter coefficients */
            std::vector<T> coefficients;
        private:
            Error executeCpxFilter(T *input_i, T *input_q, T *output_i, T *output_q, uint32_t decimation, size_t length);
            Error executeRealFilter(T *input, T *output, uint32_t decimation, size_t length);
            uint32_t order;
            FilterType filter_type;
    };

    template <class T> class LpFirFilter: public FirFilter<T>
    {
        public:
            LpFirFilter(T fs, T f, uint32_t order);
            Error calculateCoefficients(uint32_t order, T f, T *window, T *coefficients);
    };
};

#endif /*_FILTER_HPP_*/
