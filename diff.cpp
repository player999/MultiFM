#include "diff.hpp"
#include "errno.h"
#include <cmath>

namespace DSP
{
    template <class T> Error diffangle(T *omega, T *domega, size_t len)
    {
        size_t ii;
        T dPI = 2 * M_PI;
        for(ii = 0; ii < len - 1; ii++)
        {
            domega[ii] = omega[ii+1] - omega[ii];
            if(domega[ii] > M_PI)
            {
                domega[ii] -= dPI;
            }
            else if(domega[ii] < -M_PI)
            {
                domega[ii] += dPI;
            }
        }
        domega[ii] = omega[ii - 1];
        return SUCCESS;
    }

    Error diffangle(double *omega, double *domega, size_t len)
    {
        return diffangle<double>(omega, domega, len);
    }

    Error diffangle(float *omega, float *domega, size_t len)
    {
        return diffangle<float>(omega, domega, len);
    }
}

extern "C"
{
    int diffangleDbl(double *omega, double *domega, size_t len)
    {
        DSP::Error err = DSP::diffangle<double>(omega, domega, len);
        if(err == DSP::SUCCESS)
        {
            return 0;
        }
        else
        {
            return EINVAL;
        }
    }

    int diffangleFloat(float *omega, float *domega, size_t len)
    {
        DSP::Error err = DSP::diffangle<float>(omega, domega, len);
        if(err == DSP::SUCCESS)
        {
            return 0;
        }
        else
        {
            return EINVAL;
        }
    }
}