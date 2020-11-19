#include "mixer.hpp"
#include <cmath>
#include <exception>
#include "errno.h"

using namespace DSP;

namespace DSP
{
    template <class T> Error mix(T fs, T f, const T *input_i, const T *input_q,
        T *output_i, T *output_q, size_t len)
    {
        size_t ii;

        if(f > fs)
        {
            return FAIL;
        }

        T step = 2 * M_PI * f / fs;
        T current_angle = 0.0;

        for(ii = 0; ii < len; ii++)
        {
            T sin_i = sin(step * ii);
            T sin_q = cos(step * ii);

            output_i[ii] = (sin_i * input_i[ii]) - (sin_q * input_q[ii]);
            output_q[ii] = (sin_i * input_q[ii]) + (sin_q * input_i[ii]);

            current_angle += step;
            if(current_angle > (2 * M_PI))
            {
                current_angle -= 2 * M_PI;
            }
        }
        return SUCCESS;
    }

    Error mix(double fs, double f, const double *input_i, const double *input_q, double *output_i, double *output_q, size_t len)
    {
        return mix<double>(fs, f, input_i, input_q, output_i, output_q, len);
    }

    Error mix(float fs, float f, const float *input_i, const float *input_q, float *output_i, float *output_q, size_t len)
    {
        return mix<float>(fs, f, input_i, input_q, output_i, output_q, len);
    }

    template <class T> Error Mixer<T>::resizeFrame(size_t length)
    {
        T current_angle = 0.0;
        size_t start = sine_i.size();

        sine_i.resize(length);
        sine_q.resize(length);

        for(size_t ii = start; ii < length; ii++)
        {
            sine_i[ii] = sin(current_angle);
            sine_q[ii] = cos(current_angle);

            current_angle += step;
            if(current_angle > (2 * M_PI))
            {
                current_angle -= 2 * M_PI;
            }
        }

        return SUCCESS;
    }

    template <class T> Mixer<T>::Mixer(double fs, double f, size_t initial_length)
    {
        step = 2 * M_PI * f / fs;
        if(SUCCESS != resizeFrame(initial_length))
        {
            std::__throw_runtime_error("Failed to fill initial frame");
        }
    }

    template <class T> Error Mixer<T>::mix(const T *input_i, const T *input_q, T *output_i, T *output_q, size_t len)
    {
        if(len > sine_i.size())
        {
            if(SUCCESS != resizeFrame(len))
            {
                return FAIL;
            }
        }

        for(size_t ii = 0; ii < len; ii++)
        {
            output_i[ii] = (sine_i[ii] * input_i[ii]) - (sine_q[ii] * input_q[ii]);
            output_q[ii] = (sine_i[ii] * input_q[ii]) + (sine_q[ii] * input_i[ii]);
        }
        return SUCCESS;
    }
}

template class Mixer<float>;
template class Mixer<double>;

extern "C"
{
    int mixDbl(double fs, double f, const double *input_i, const double *input_q, double *output_i, double *output_q, size_t len)
    {
        Error err = mix<double>(fs, f, input_i, input_q, output_i, output_q, len);
        if(err == SUCCESS)
        {
            return 0;
        }
        else
        {
            return EINVAL;
        }
    }

    int mixFloat(float fs, float f, const float *input_i, const float *input_q, float *output_i, float *output_q, size_t len)
    {
        Error err = mix<float>(fs, f, input_i, input_q, output_i, output_q, len);
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