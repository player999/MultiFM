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

    class FirFilter
    {
        public:
            FirFilter(FilterType ft, float fs, float f1, float f2, uint32_t order);
            static Error getWindow(WindowFunction window_type, uint32_t order, float *coefficients);
            Error execute(float *input_i, float *input_q, float *output_i, float *output_q, size_t length);
            Error executeRealFilter(float *input_i, float *input_q, float *output_i, float *output_q, size_t length);
            uint32_t order;
            float sampling_rate;
            float f1, f2;
            FilterType filter_type;
            std::vector<float> window;
            std::vector<float> coefficients;
    };

    class LpFirFilter: public FirFilter
    {
        public:
            LpFirFilter(float fs, float f, uint32_t order);
            Error calculateCoefficients(uint32_t order, float f, float *window, float *coefficients);
    };
};

#endif /*_FILTER_HPP_*/