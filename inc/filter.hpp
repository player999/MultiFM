#ifndef _FILTER_HPP_
#define _FILTER_HPP_
#include <vector>
#include <tuple>
#include <cstdint>
#include "error.hpp"

namespace DSP
{
    enum FilterType
    {
        FILTER_TYPE_LP,
        FILTER_TYPE_HP,
        FILTER_TYPE_BP,
    };

    enum WindowFunction
    {
        BARTLETT,
        HANNING,
        BLACKMAN,
        HAMMING,
        BLACKMAN_HARRIS,
        BLACKMAN_NUTTAL,
        NUTTAL,
    };

    template <class T> class FirFilter
    {
        public:
            FirFilter(FilterType ft, T fs, T f1, T f2, uint32_t order);
            static Error getWindow(WindowFunction window_type, uint32_t order, T *coefficients);
            Error executeCpx(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            Error executeCpxDecim(T *input_i, T *input_q, T *output_i, T *output_q, uint32_t decim, size_t length);
            Error executeReal(T *input, T *output, size_t length);
            Error executeRealDecim(T *input, T *output, uint32_t decim, size_t length);
            T sampling_rate;
            T f1, f2;
            std::vector<T> window;
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