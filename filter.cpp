#include <algorithm>
#include <cmath>
#include <exception>
#include <functional>
#include <filter.hpp>


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

template<class T> Error FirFilter<T>::execute(T *input_i, T *input_q, T *output_i, T *output_q, size_t length)
{
    if(filter_type == FilterType::FILTER_TYPE_LP)
    {
        return executeRealFilter(input_i, input_q, output_i, output_q, length);
    }
    return Error::SUCCESS;
}

template <class T> Error FirFilter<T>::executeRealFilter(T *input_i, T *input_q, T *output_i, T *output_q, size_t length)
{
    int32_t ncoef;
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
        output_i[ii] = 0.0;
        output_q[ii] = 0.0;
        for(int32_t jj = -ii; jj < ncoef; jj++)
        {
            output_i[ii] += (input_i[ii + jj] * c[jj + ncoef]);
            output_q[ii] += (input_q[ii + jj] * c[jj + ncoef]);
        }
    }

    /* Middle */
    for(int32_t ii = (coefficients.size() - ncoef); ii < length - ncoef + 1; ii++)
    {
        output_i[ii] = 0.0;
        output_q[ii] = 0.0;
        for(int32_t jj = -ncoef; jj < ncoef; jj++)
        {
            output_i[ii] += (input_i[ii + jj] * c[jj + ncoef]);
            output_q[ii] += (input_q[ii + jj] * c[jj + ncoef]);
        }
    }

    /* Convolve last part */
    for(int32_t ii = length - ncoef + 1; ii < length; ii++)
    {
        output_i[ii] = 0.0;
        output_q[ii] = 0.0;
        for(int32_t jj = -ncoef; jj < ncoef - ((int32_t)length - ii); jj++)
        {
            output_i[ii] += (input_i[ii + jj] * c[jj + ncoef]);
            output_q[ii] += (input_q[ii + jj] * c[jj + ncoef]);
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
