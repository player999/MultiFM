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
#include "diff.h"
#include "mex.h"
#include "matrix.h"


/*int diffangleDbl(double *omega, double *domega, size_t len);*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *domega, *omega;
    size_t len;

    /* check for proper number of arguments */
    if(nrhs != 1)
    {
        mexErrMsgIdAndTxt("CuteDSP:Diff:nargs", "Strictly one input required.");
        goto L_exit;
    }

    if(nlhs != 1)
    {
        mexErrMsgIdAndTxt("CuteDSP:Diff:nargs", "Strictly one output required.");
        goto L_exit;
    }

    if(!mxIsDouble(prhs[0]))
    {
        mexErrMsgIdAndTxt("CuteDSP:Diff:type", "Expect double!");
        goto L_exit;
    }

    /* Inputs */
    len = mxGetNumberOfElements(prhs[0]);
    omega = mxGetPr(prhs[0]);

    plhs[0] = mxCreateDoubleMatrix(1, len, mxREAL);
    domega = mxGetPr(plhs[0]);

    if(diffangleDbl(omega, domega, len))
    {
        mexErrMsgIdAndTxt("CuteDSP:Ataniq:execution", "Failed to run atan()");
    }
L_exit:
    return;
}