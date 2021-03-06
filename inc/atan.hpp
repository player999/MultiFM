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
#ifndef _ATAN_HPP_
#define _ATAN_HPP_

#include "error.hpp"
#include <cstdlib>
namespace DSP
{
    /** @brief Arctan from two cathetuses for double
     *  @param [in] in_i I-data.
     *  @param [in] in_q Q-data.
     *  @param [out] omega resulting angles.
     *  @param [in] len length of data
     */
    Error ataniq(double *in_i, double *in_q, double *omega, size_t len);

    /** @brief Arctan from two cathetuses for float
     *  @param [in] in_i I-data.
     *  @param [in] in_q Q-data.
     *  @param [out] omega resulting angles.
     *  @param [in] len length of data
     */
    Error ataniq(float *in_i, float *in_q, float *omega, size_t len);
}

#endif /*_ATAN_HPP_*/