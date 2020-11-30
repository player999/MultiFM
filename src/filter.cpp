/******************************************************************************
Copyright 2020 Taras Zakharchenko taras.zakharchenko@gmail.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1.  Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
3.  Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
#include <algorithm>
#include <cmath>
#include <exception>
#include <functional>
#include <filter.hpp>

extern "C"
{
    #include <filter.h>
}


using namespace DSP;

template <class T> FirFilter<T>::FirFilter(FilterType ft, T fs, T f1, T f2, uint32_t order):
    f1(f1), f2(f2), order(order), filter_type(ft), sampling_rate(fs)
{
    Error err;
    window.resize(order);
    err = getWindow(WindowFunction::HAMMING, order, window.data());
    if(Error::SUCCESS != err) std::__throw_runtime_error("Failed to initialize filter window");
    coefficients.resize(order);
}

template <class T> FirFilter<T>::FirFilter(std::vector<double> &cfs)
{
    order = cfs.size();
    filter_type = FILTER_TYPE_CUSTOM;
    std::copy(cfs.begin(), cfs.end(), std::back_inserter(coefficients));
}

template <class T> Error FirFilter<T>::getWindow(WindowFunction window_type, uint32_t order, T *coefficients)
{
    std::function<T(uint32_t)> window_functor;
    if(WindowFunction::HAMMING == window_type)
    {
        window_functor = [order](uint32_t i) {
            T a = 2.0 * M_PI * i / (order - 1);
            return 0.54 - 0.46 * std::cos(a);
        };
    }
    else
    {
        return Error::FAIL;
    }

    for(uint32_t jj = 0; jj < order; jj++)
    {
        coefficients[jj] = window_functor(jj);
    }

    return Error::SUCCESS;
}

template<class T> Error FirFilter<T>::executeRealDecim(T *input, T *output, uint32_t decimation, size_t length)
{
    if((filter_type == FilterType::FILTER_TYPE_LP) || (filter_type == FilterType::FILTER_TYPE_CUSTOM))
    {
        return executeRealFilter(input, output, decimation, length);
    }
    return Error::SUCCESS;
}

template<class T> Error FirFilter<T>::executeReal(T *input, T *output, size_t length)
{
    return executeRealDecim(input, output, 1, length);
}

template <class T> Error FirFilter<T>::executeRealFilter(T *input, T *output, uint32_t decimation, size_t length)
{
    int32_t ncoef;
    size_t ooffset = 0;
    if((coefficients.size() % 2) == 0)
    {
        ncoef = coefficients.size() / 2;
    }
    else
    {
        ncoef = ((coefficients.size() / 2) + 1);
    }

    std::vector<T> c(coefficients);
    std::reverse(c.begin(), c.end());

    /* Convolve first part */
    for(int32_t ii = 0; ii < (coefficients.size() - ncoef); ii++)
    {
        if((ii % decimation) == 0)
        {
            output[ooffset] = 0.0;
            for(int32_t jj = -ii; jj < ncoef; jj++)
            {
                output[ooffset] += (input[ii + jj] * c[jj + ncoef]);
            }
            ooffset++;
        }
    }

    /* Middle */
    for(int32_t ii = (coefficients.size() - ncoef); ii < length - ncoef + 1; ii++)
    {
        if((ii % decimation) == 0)
        {
            output[ooffset] = 0.0;
            for(int32_t jj = -ncoef; jj < ncoef; jj++)
            {
                output[ooffset] += (input[ii + jj] * c[jj + ncoef]);
            }
            ooffset++;
        }
    }

    /* Convolve last part */
    for(int32_t ii = length - ncoef + 1; ii < length; ii++)
    {
        if((ii % decimation) == 0)
        {
            output[ooffset] = 0.0;
            for(int32_t jj = -ncoef; jj < ncoef - ((int32_t)length - ii); jj++)
            {
                output[ooffset] += (input[ii + jj] * c[jj + ncoef]);
            }
            ooffset++;
        }
    }

    return Error::SUCCESS;
}

template<class T> Error FirFilter<T>::executeCpxDecim(T *input_i, T *input_q, T *output_i, T *output_q, uint32_t decimation, size_t length)
{
    if(filter_type == FilterType::FILTER_TYPE_LP)
    {
        return executeCpxFilter(input_i, input_q, output_i, output_q, decimation, length);
    }
    return Error::SUCCESS;
}

template<class T> Error FirFilter<T>::executeCpx(T *input_i, T *input_q, T *output_i, T *output_q, size_t length)
{
    return executeCpxDecim(input_i, input_q, output_i, output_q, 1, length);
}

template <class T> Error FirFilter<T>::executeCpxFilter(T *input_i, T *input_q, T *output_i, T *output_q, uint32_t decimation, size_t length)
{
    int32_t ncoef;
    size_t ooffset = 0;
    if((coefficients.size() % 2) == 0)
    {
        ncoef = coefficients.size() / 2;
    }
    else
    {
        ncoef = ((coefficients.size() / 2) + 1);
    }

    std::vector<T> c(coefficients);
    std::reverse(c.begin(), c.end());

    /* Convolve first part */
    for(int32_t ii = 0; ii < (coefficients.size() - ncoef); ii++)
    {
        if((ii % decimation) == 0)
        {
            output_i[ooffset] = 0.0;
            output_q[ooffset] = 0.0;
            for(int32_t jj = -ii; jj < ncoef; jj++)
            {
                output_i[ooffset] += (input_i[ii + jj] * c[jj + ncoef]);
                output_q[ooffset] += (input_q[ii + jj] * c[jj + ncoef]);
            }
            ooffset++;
        }
    }

    /* Middle */
    for(int32_t ii = (coefficients.size() - ncoef); ii < length - ncoef + 1; ii++)
    {
        if((ii % decimation) == 0)
        {
            output_i[ooffset] = 0.0;
            output_q[ooffset] = 0.0;
            for(int32_t jj = -ncoef; jj < ncoef; jj++)
            {
                output_i[ooffset] += (input_i[ii + jj] * c[jj + ncoef]);
                output_q[ooffset] += (input_q[ii + jj] * c[jj + ncoef]);
            }
            ooffset++;
        }
    }

    /* Convolve last part */
    for(int32_t ii = length - ncoef + 1; ii < length; ii++)
    {
        if((ii % decimation) == 0)
        {
            output_i[ooffset] = 0.0;
            output_q[ooffset] = 0.0;
            for(int32_t jj = -ncoef; jj < ncoef - ((int32_t)length - ii); jj++)
            {
                output_i[ooffset] += (input_i[ii + jj] * c[jj + ncoef]);
                output_q[ooffset] += (input_q[ii + jj] * c[jj + ncoef]);
            }
            ooffset++;
        }
    }

    return Error::SUCCESS;
}

template <class T> LpFirFilter<T>::LpFirFilter(T fs, T f, uint32_t order):
    FirFilter<T>(FILTER_TYPE_LP, fs, f, 0.0f, order)
{
    Error err;
    err = calculateCoefficients(order, f, this->window.data(), this->coefficients.data());
    if(Error::SUCCESS != err) std::__throw_runtime_error("Failed to calculate filter coefficients");
}

template <class T> Error LpFirFilter<T>::calculateCoefficients(uint32_t order, T f, T *window, T *coefficients)
{
    if( order == 1 )
    {
        coefficients[0] = 1.0;
        return Error::SUCCESS;
    }
    uint32_t n2 = order / 2;

    T w = 2.0 * M_PI * this->f1 / this->sampling_rate;
    T sum = 0;

    for(uint32_t ii = 0; ii < order; ii++ ) {
            int32_t d = ii - n2;
            coefficients[ii] = ((d == 0) ? w : std::sin(w * d) / d) * window[ii];
            sum += coefficients[ii];
    }

    for(uint32_t ii = 0; ii < order; ii++ )
    {
        coefficients[ii] /= sum;
    }

    return Error::SUCCESS;
}

template class LpFirFilter<float>;
template class LpFirFilter<double>;
template class FirFilter<float>;
template class FirFilter<double>;

extern "C"
{
    int filterDbl(uint8_t ftype, uint32_t order, double fs, double f, double *ii, double *iq, double *oi, double *oq, uint32_t decim, size_t len)
    {
        if(FILTER_C_TYPE_LP == ftype)
        {
            LpFirFilter<double> lp(fs, f, order);
            if(lp.executeCpxDecim(ii, iq, oi, oq, decim, len))
            {
                return DSP::Error::FAIL;
            }
            return DSP::Error::SUCCESS;
        }
        else
        {
            /* Filter type is not supported */
            return DSP::Error::FAIL;
        }
    }
}