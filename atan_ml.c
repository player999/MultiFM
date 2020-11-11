#include <string.h>
#include <stdint.h>
#include "atan.h"
#include "mex.h"
#include "matrix.h"


/*int ataniqDbl(double *in_i, double *in_q, double *omega, size_t len);*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *ii, *iq, *omega;
    size_t len;
    uint8_t offset = 0;

    /* check for proper number of arguments */
    if(nrhs != 2)
    {
        mexErrMsgIdAndTxt("CuteDSP:Ataniq:nargs", "Strictly two inputs required.");
        goto L_exit;
    }

    if(nlhs != 1)
    {
        mexErrMsgIdAndTxt("CuteDSP:Ataniq:nargs", "Strictly one output required.");
        goto L_exit;
    }

    if(!mxIsDouble(prhs[0]) || !mxIsDouble(prhs[1]))
    {
        mexErrMsgIdAndTxt("CuteDSP:Ataniq:type", "Expect double!");
        goto L_exit;
    }

    /* Inputs */
    if(mxGetNumberOfElements(prhs[offset]) != mxGetNumberOfElements(prhs[offset + 1]))
    {
        mexErrMsgIdAndTxt("CuteDSP:Ataniq:IQ", "Elements number is unaligned between I and Q components");
        goto L_exit;
    }
    len = mxGetNumberOfElements(prhs[offset]);

    ii = mxGetPr(prhs[0]);
    iq = mxGetPr(prhs[1]);

    plhs[0] = mxCreateDoubleMatrix(1, len, mxREAL);
    omega = mxGetPr(plhs[0]);

    if(ataniqDbl(ii, iq, omega, len))
    {
        mexErrMsgIdAndTxt("CuteDSP:Ataniq:execution", "Failed to run atan()");
    }
L_exit:
    return;
}