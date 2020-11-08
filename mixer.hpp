#ifndef _MIXER_HPP_
#define _MIXER_HPP_

#include "error.hpp"
#include <cstdlib>

namespace DSP
{
    Error mix(double fs, double f, double *input_i, double *input_q, double *output_i, double *output_q, size_t len);
}

#endif /*_MIXER_HPP_*/