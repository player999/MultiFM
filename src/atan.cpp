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

#include "atan.hpp"
#include <cmath>
#include <errno.h>

using namespace DSP;

namespace DSP
{
    template <typename T> Error ataniq(T *in_i, T *in_q, T *omega, size_t len)
    {
        for(size_t ii = 0; ii < len; ii++)
        {
            T ovalue = std::atan2(in_q[ii], in_i[ii]);
            omega[ii] = ovalue;
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
