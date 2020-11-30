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
#include "audio_encoder.h"
#include "mex.h"
#include "matrix.h"


/*int diffangleDbl(double *omega, double *domega, size_t len);*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *data;
    size_t len;
    char filename[256];

    /* check for proper number of arguments */
    if(nrhs != 2)
    {
        mexErrMsgIdAndTxt("CuteDSP:Coder:nargs", "Strictly one input required.");
        goto L_exit;
    }

    if(nlhs != 0)
    {
        mexErrMsgIdAndTxt("CuteDSP:Coder:nargs", "No outputs expected");
        goto L_exit;
    }

    if(!mxIsChar(prhs[0]))
    { 
        mexErrMsgIdAndTxt("CuteDSP:Coder:FilterType", "Filter name must be a string.");
        goto L_exit;
    }

    if(!mxIsDouble(prhs[1]))
    {
        mexErrMsgIdAndTxt("CuteDSP:Coder:type", "Expect double!");
        goto L_exit;
    }

    if(0 != mxGetString(prhs[0], filename, sizeof(filename)))
    {
        mexErrMsgIdAndTxt("CuteDSP:Coder:FileName", "Failed to get string with file name");
        goto L_exit;
    }

    len = mxGetNumberOfElements(prhs[1]);
    data = mxGetPr(prhs[1]);

    audioCodeDbl(filename, data, len);
L_exit:
    return;
}