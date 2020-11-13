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