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
#include <string.h>
#include <stdint.h>
#include "mixer.h"
#include "mex.h"
#include "matrix.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *ii, *iq, *oi, *oq;
    size_t len;
    double fs = 0, f = 0;
    uint8_t offset = 0;

    /* check for proper number of arguments */
    if(nrhs != 4)
    {
        mexErrMsgIdAndTxt("CuteDSP:Mixer:nargs", "Strictly four inputs required.");
        goto L_exit;
    }

    if(nlhs!=2)
    {
        mexErrMsgIdAndTxt("CuteDSP:Mixer:nlhs", "Two outputs required.");
        goto L_exit;
    }

    /* Sampling frequency */
    if(!mxIsDouble(prhs[offset]) || (mxGetNumberOfElements(prhs[offset]) != 1))
    {
        mexErrMsgIdAndTxt("CuteDSP:Mixer:sampling_frequency", "Sampling frequency is bad");
        goto L_exit;
    }
    else
    {
        fs = mxGetScalar(prhs[offset]);
        offset++;
    }

    /* Filtering frequency */
    if(!mxIsDouble(prhs[offset]) || (mxGetNumberOfElements(prhs[offset]) != 1))
    {
        mexErrMsgIdAndTxt("CuteDSP:Mixer:frequency", "Mixing frequency");
        goto L_exit;
    }
    else
    {
        f = mxGetScalar(prhs[offset]);
        offset++;
    }

    /* Inputs */
    if(mxGetNumberOfElements(prhs[offset]) != mxGetNumberOfElements(prhs[offset + 1]))
    {
        mexErrMsgIdAndTxt("CuteDSP:Mixer:IQ", "Elements number is unaligned between I and Q components");
        goto L_exit;
    }
    len = mxGetNumberOfElements(prhs[offset]);

    ii = mxGetPr(prhs[offset++]);
    iq = mxGetPr(prhs[offset++]);

    plhs[0] = mxCreateDoubleMatrix(1, len, mxREAL);
    plhs[1] = mxCreateDoubleMatrix(1, len, mxREAL);

    oi = mxGetPr(plhs[0]);
    oq = mxGetPr(plhs[1]);
    
    if(mixDbl(fs, f, ii, iq, oi, oq, len))
    {
        mexErrMsgIdAndTxt("CuteDSP:Mixer:execution", "Failed to run mixer");
    }

L_exit:
    return;
}