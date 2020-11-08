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