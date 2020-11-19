#ifndef _DIFF_HPP_
#define _DIFF_HPP_

#include "error.hpp"
#include <cstdlib>
namespace DSP
{
    Error diffangle(double *omega, double *domega, size_t len);
    Error diffangle(float *omega, float *domega, size_t len);
}

#endif /*_DIFF_HPP_*/