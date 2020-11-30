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
#include "interleaving.hpp"

namespace DSP
{
    template <typename T, typename F> Error deinterleave(T *buffer, size_t total_elems, F *output_i, F *output_q)
    {
        size_t offset;

        if(total_elems % 2)
        {
            return FAIL;
        }

        offset = 0;
        for(size_t ii = 0; ii < total_elems / 2; ii++)
        {
            output_i[ii] = (F)buffer[offset++];
            output_q[ii] = (F)buffer[offset++];
        }

        return SUCCESS;
    }

    Error deinterleave(int8_t *buffer, size_t total_elems, double *output_i, double *output_q)
    {
        return deinterleave<int8_t, double>(buffer, total_elems, output_i, output_q);
    }

    Error deinterleave(int8_t *buffer, size_t total_elems, float *output_i, float *output_q)
    {
        return deinterleave<int8_t, float>(buffer, total_elems, output_i, output_q);
    }

    Error deinterleave(double *buffer, size_t total_elems, double *output_i, double *output_q)
    {
        return deinterleave<double, double>(buffer, total_elems, output_i, output_q);
    }

    Error deinterleave(float *buffer, size_t total_elems, float *output_i, float *output_q)
    {
        return deinterleave<float, float>(buffer, total_elems, output_i, output_q);
    }


    template <typename T, typename F> Error deinterleave(std::vector<T> &buffer, F *output_i, F *output_q)
    {
        size_t offset;

        if(buffer.size() % 2)
        {
            return FAIL;
        }

        offset = 0;
        for(size_t ii = 0; ii < buffer.size() / 2; ii++)
        {
            output_i[ii] = (F)buffer[offset++];
            output_q[ii] = (F)buffer[offset++];
        }

        return SUCCESS;
    }

    Error deinterleave(std::vector<int8_t> &buffer, double *output_i, double *output_q)
    {
        return deinterleave<int8_t, double>(buffer, output_i, output_q);
    }

    Error deinterleave(std::vector<int8_t> &buffer, float *output_i, float *output_q)
    {
        return deinterleave<int8_t, float>(buffer, output_i, output_q);
    }

    Error deinterleave(std::vector<double> &buffer, double *output_i, double *output_q)
    {
        return deinterleave<double, double>(buffer, output_i, output_q);
    }

    Error deinterleave(std::vector<float> &buffer, float *output_i, float *output_q)
    {
        return deinterleave<float, float>(buffer, output_i, output_q);
    }

    template <typename T, typename F> Error deinterleave(std::vector<T> &buffer, std::vector<F> &obuffer_i, std::vector<F> &obuffer_q)
    {
        size_t offset;

        if(buffer.size() % 2)
        {
            return FAIL;
        }

        obuffer_i.resize(buffer.size() / 2);
        obuffer_q.resize(buffer.size() / 2);

        offset = 0;
        for(size_t ii = 0; ii < buffer.size() / 2; ii++)
        {
            obuffer_i[ii] = (F)buffer[offset++];
            obuffer_q[ii] = (F)buffer[offset++];
        }

        return SUCCESS;
    }

    Error deinterleave(std::vector<int8_t> &buffer, std::vector<double> &obuffer_i, std::vector<double> &obuffer_q)
    {
        return deinterleave<int8_t, double>(buffer, obuffer_i, obuffer_q);
    }

    Error deinterleave(std::vector<int8_t> &buffer, std::vector<float> &obuffer_i, std::vector<float> &obuffer_q)
    {
        return deinterleave<int8_t, float>(buffer, obuffer_i, obuffer_q);
    }

    Error deinterleave(std::vector<double> &buffer, std::vector<double> &obuffer_i, std::vector<double> &obuffer_q)
    {
        return deinterleave<double, double>(buffer, obuffer_i, obuffer_q);
    }

    Error deinterleave(std::vector<float> &buffer, std::vector<float> &obuffer_i, std::vector<float> &obuffer_q)
    {
        return deinterleave<float, float>(buffer, obuffer_i, obuffer_q);
    }

    template <typename T, typename F> Error deinterleave(T *buffer, size_t total_elems, std::vector<F> &obuffer_i, std::vector<F> &obuffer_q)
    {
        size_t offset;

        if(total_elems % 2)
        {
            return FAIL;
        }

        obuffer_i.resize(total_elems / 2);
        obuffer_q.resize(total_elems/ 2);
        offset = 0;
        for(size_t ii = 0; ii < total_elems / 2; ii++)
        {
            obuffer_i[ii] = (F)buffer[offset++];
            obuffer_q[ii] = (F)buffer[offset++];
        }

        return SUCCESS;
    }

    Error deinterleave(int8_t *buffer, size_t total_elems, std::vector<double> &obuffer_i, std::vector<double> &obuffer_q)
    {
        return deinterleave<int8_t, double>(buffer, total_elems, obuffer_i, obuffer_q);
    }

    Error deinterleave(int8_t *buffer, size_t total_elems, std::vector<float> &obuffer_i, std::vector<float> &obuffer_q)
    {
        return deinterleave<int8_t, float>(buffer, total_elems, obuffer_i, obuffer_q);
    }

    Error deinterleave(double *buffer, size_t total_elems, std::vector<double> &obuffer_i, std::vector<double> &obuffer_q)
    {
        return deinterleave<double, double>(buffer, total_elems, obuffer_i, obuffer_q);
    }

    Error deinterleave(float *buffer, size_t total_elems, std::vector<float> &obuffer_i, std::vector<float> &obuffer_q)
    {
        return deinterleave<float, float>(buffer, total_elems, obuffer_i, obuffer_q);
    }
}
