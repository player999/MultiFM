#ifndef _ATAN_HPP_
#define _ATAN_HPP_

#include "error.hpp"
#include <cstdlib>
namespace DSP
{
    Error ataniq(double *in_i, double *in_q, double *omega, size_t len);
    Error ataniq(float *in_i, float *in_q, float *omega, size_t len);
}

#endif /*_ATAN_HPP_*/