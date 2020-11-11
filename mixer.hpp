#ifndef _MIXER_HPP_
#define _MIXER_HPP_

#include "error.hpp"
#include <cstdlib>

namespace DSP
{
    Error mix(double fs, double f, double *input_i, double *input_q, double *output_i, double *output_q, size_t len);
    Error mix(float fs, float f, float *input_i, float *input_q, float *output_i, float *output_q, size_t len);
}

#endif /*_MIXER_HPP_*/