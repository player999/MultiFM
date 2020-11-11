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