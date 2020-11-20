#include <cstdint>
#include <cmath>
#include "fft.hpp"

namespace DSP
{
    template <class T> Error FFT<T>::updateW(size_t s, bool forward)
    {
        if(true == forward)
        {
            size_t old_size = wi.size();
            if(wi.size() > s) return Error::SUCCESS;
            old_size = wi.size();
            wi.resize(s);
            wq.resize(s);
            for (size_t ii = old_size; ii < s; ii++)
            {
                wi[ii] = cos(-2 * M_PI * ii / s);
                wq[ii] = sin(-2 * M_PI * ii / s);
            }
        }
        else
        {
            size_t old_size = inverse_wi.size();
            if(inverse_wi.size() > s) return Error::SUCCESS;
            old_size = inverse_wi.size();
            inverse_wi.resize(s);
            inverse_wq.resize(s);
            for (size_t ii = old_size; ii < s; ii++)
            {
                inverse_wi[ii] = cos(2 * M_PI * ii / s);
                inverse_wq[ii] = sin(2 * M_PI * ii / s);
            }
        }

        return Error::SUCCESS;
    }

    template <class T> size_t FFT<T>::singleBitrev(size_t idx, uint8_t w)
    {
        size_t res = 0;
        for (int ii = 0; ii < w; ii++, idx >>= 1)
        {
            res = (res << 1) | (idx & 1); 
        }
        return res;
    }

    template <class T> void FFT<T>::updateBitrev(size_t s)
    {
        uint8_t width = std::log2(s);
        bitrev.resize(s);
        for (int jj = 0; jj < s; jj++)
        {
            bitrev[jj] = singleBitrev(jj, width);
        }
    }

    template <class T> FFT<T>::FFT()
    {

    }

    template <class T> Error FFT<T>::transform2(T *input_i, T *input_q, T *output_i, T *output_q, size_t length)
    {
        uint32_t iterations = 0;
        Error err;
        for (size_t temp = length; temp > 1; temp >>= 1) iterations++;
        if((1 << iterations) != length) return Error::FAIL;//Not a power of 2
        if(length != wi.size()) updateW(8, true);
        if(length != bitrev.size()) updateBitrev(length);

        for(size_t ii = 0; ii < length; ii++)
        {
            output_i[ii] = input_i[bitrev[ii]];
            output_q[ii] = input_q[bitrev[ii]];
        }

        for (size_t size = 2; size <= length; size *= 2)
        {
            size_t halfsize = size / 2;
            size_t tablestep = length / size;
            for (size_t ii = 0; ii < length; ii += size)
            {
                for (size_t jj = ii, kk = 0; jj < ii + halfsize; jj++, kk += tablestep)
                {
                    size_t ll = jj + halfsize;
                    T temp_i = (output_i[ll] * wi[kk]) - (output_q[ll] * wq[kk]);
                    T temp_q = (output_i[ll] * wq[kk]) + (output_q[ll] * wi[kk]);
                    output_i[ll] = output_i[jj] - temp_i;
                    output_q[ll] = output_q[jj] - temp_q;
                    output_i[jj] = output_i[jj] + temp_i;
                    output_q[jj] = output_q[jj] + temp_q;
                }
            }
            if (size == length)
            {
                break;
            }
        }
        return Error::SUCCESS;
    }

    template <class T> Error FFT<T>::inverse_transform2(T *input_i, T *input_q, T *output_i, T *output_q, size_t length)
    {
        uint32_t iterations = 0;
        Error err;
        for (size_t temp = length; temp > 1; temp >>= 1) iterations++;
        if((1 << iterations) != length) return Error::FAIL;
        if(length != inverse_wi.size()) updateW(8, false);
        if(length != bitrev.size()) updateBitrev(length);

        for(size_t ii = 0; ii < length; ii++)
        {
            output_i[ii] = input_i[bitrev[ii]];
            output_q[ii] = input_q[bitrev[ii]];
        }

        for (size_t size = 2; size <= length; size *= 2)
        {
            size_t halfsize = size / 2;
            size_t tablestep = length / size;
            for (size_t ii = 0; ii < length; ii += size)
            {
                for (size_t jj = ii, kk = 0; jj < ii + halfsize; jj++, kk += tablestep)
                {
                    size_t ll = jj + halfsize;
                    T temp_i = (output_i[ll] * inverse_wi[kk]) - (output_q[ll] * inverse_wq[kk]);
                    T temp_q = (output_i[ll] * inverse_wq[kk]) + (output_q[ll] * inverse_wi[kk]);
                    output_i[ll] = output_i[jj] - temp_i;
                    output_q[ll] = output_q[jj] - temp_q;
                    output_i[jj] = output_i[jj] + temp_i;
                    output_q[jj] = output_q[jj] + temp_q;
                }
            }
            if (size == length)
            {
                break;
            }
        }

        for(size_t ii = 0; ii < length; ii++)
        {
            output_i[ii] /= length;
            output_q[ii] /= length;
        }
        return Error::SUCCESS;
    }

    // template <class T> Error fft_transform_bluestein(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
    // template <class T> Error fft_inverse_transform_bluestein(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
    // template <class T> Error fft_transform(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
    // template <class T> Error fft_inverse_transform(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);

    template class FFT<float>;
    template class FFT<double>;
}
