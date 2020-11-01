#include <algorithm>
#include <cmath>
#include <exception>
#include <functional>
#include <filter.hpp>


using namespace DSP;

FirFilter::FirFilter(FilterType ft, float fs, float f1, float f2, uint32_t order):
    f1(f1), f2(f2), order(order), filter_type(ft), sampling_rate(fs)
{
    Error err;
    window.resize(order);
    err = getWindow(WindowFunction::HAMMING, order, window.data());
    if(Error::SUCCESS != err) std::__throw_runtime_error("Failed to initialize filter window");
    coefficients.resize(order);
}

Error FirFilter::getWindow(WindowFunction window_type, uint32_t order, float *coefficients)
{
    std::function<float(uint32_t)> window_functor;
    if(WindowFunction::HAMMING == window_type)
    {
        window_functor = [order](uint32_t i) {
            float a = 2.0 * M_PI * i / (order - 1);
            return 0.54f - 0.46f * std::cos(a);
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

Error FirFilter::execute(float *input_i, float *input_q, float *output_i, float *output_q, size_t length)
{
    if(filter_type == FilterType::FILTER_TYPE_LP)
    {
        return executeRealFilter(input_i, input_q, output_i, output_q, length);
    }
    return Error::SUCCESS;
}

Error FirFilter::executeRealFilter(float *input_i, float *input_q, float *output_i, float *output_q, size_t length)
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

    std::vector<float> c(coefficients);
    std::reverse(c.begin(), c.end());

    /* Convolve first part */
    for(int32_t ii = 0; ii < (coefficients.size() - ncoef); ii++)
    {
        output_i[ii] = 0.0f;
        output_q[ii] = 0.0f;
        for(int32_t jj = -ii; jj < ncoef; jj++)
        {
            output_i[ii] += (input_i[ii + jj] * c[jj + ncoef]);
            output_q[ii] += (input_q[ii + jj] * c[jj + ncoef]);
        }
    }

    /* Middle */
    for(int32_t ii = (coefficients.size() - ncoef); ii < length - ncoef + 1; ii++)
    {
        output_i[ii] = 0.0f;
        output_q[ii] = 0.0f;
        for(int32_t jj = -ncoef; jj < ncoef; jj++)
        {
            output_i[ii] += (input_i[ii + jj] * c[jj + ncoef]);
            output_q[ii] += (input_q[ii + jj] * c[jj + ncoef]);
        }
    }

    /* Convolve last part */
    for(int32_t ii = length - ncoef + 1; ii < length; ii++)
    {
        output_i[ii] = 0.0f;
        output_q[ii] = 0.0f;
        for(int32_t jj = -ncoef; jj < ncoef - ((int32_t)length - ii); jj++)
        {
            output_i[ii] += (input_i[ii + jj] * c[jj + ncoef]);
            output_q[ii] += (input_q[ii + jj] * c[jj + ncoef]);
        }
    }

    return Error::SUCCESS;
}

LpFirFilter::LpFirFilter(float fs, float f, uint32_t order):
    FirFilter(FILTER_TYPE_LP, fs, f, 0.0f, order)
{
    Error err;
    err = calculateCoefficients(order, f, window.data(), coefficients.data());
    if(Error::SUCCESS != err) std::__throw_runtime_error("Failed to calculate filter coefficients");
}

Error LpFirFilter::calculateCoefficients(uint32_t order, float f, float *window, float *coefficients)
{
    if( order == 1 )
    {
        coefficients[0] = 1.0f;
        return Error::SUCCESS;
    }
    uint32_t n2 = order / 2;

    float w = 2.0 * M_PI * f1 / sampling_rate;
    float sum = 0;

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
