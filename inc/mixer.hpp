#ifndef _MIXER_HPP_
#define _MIXER_HPP_

#include "error.hpp"
#include <cstdlib>
#include <vector>

namespace DSP
{
    template <class T> class Mixer
    {
        public:
            Mixer(double fs, double f, size_t initial_length);
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