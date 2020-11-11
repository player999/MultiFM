#include "atan.hpp"
#include <cmath>
#include <errno.h>

using namespace DSP;

template <typename T> Error ataniq(T *in_i, T *in_q, T *omega, size_t len)
{
    for(size_t ii = 0; ii < len; ii++)
    {
        T ovalue = std::atan(std::abs(in_q[ii]) / std::abs(in_i[ii]));
        if((in_i[ii] > 0.0) && (in_q[ii] > 0.0))
        {
            omega[ii] = ovalue;
        }
        else if((in_i[ii] > 0.0) && (in_q[ii] < 0.0))
        {
            omega[ii] = -ovalue;
        }
        else if((in_i[ii] < 0.0) && (in_q[ii] < 0.0))
        {
            omega[ii] = ovalue - M_PI;
        }
        else if((in_i[ii] < 0.0) && (in_q[ii] > 0.0))
        {
            omega[ii] = M_PI - ovalue;
        }
    }
    return SUCCESS;
}

template Error ataniq<double>(double *in_i, double *in_q, double *omega, size_t len);
template Error ataniq<float>(float *in_i, float *in_q, float *omega, size_t len);

Error ataniq(double *in_i, double *in_q, double *omega, size_t len)
{
    return ataniq<double>(in_i, in_q, omega, len);
}

Error ataniq(float *in_i, float *in_q, float *omega, size_t len)
{
    return ataniq<float>(in_i, in_q, omega, len);
}

extern "C"
{
    int ataniqDbl(double *in_i, double *in_q, double *omega, size_t len)
    {
        Error err = ataniq<double>(in_i, in_q, omega, len);
        if(err == SUCCESS)
        {
            return 0;
        }
        else
        {
            return EINVAL;
        }
    }

    int ataniqFloat(float *in_i, float *in_q, float *omega, size_t len)
    {
        Error err = ataniq<float>(in_i, in_q, omega, len);
        if(err == SUCCESS)
        {
            return 0;
        }
        else
        {
            return EINVAL;
        }
    }
}
