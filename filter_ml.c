#include <string.h>
#include "filter.h"
#include "mex.h"
#include "matrix.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *ii, *iq, *oi, *oq;
    size_t len;
    uint8_t filter_type;
    uint32_t order = 0;
    double fs = 0, f1 = 0, f2 = 0;
    uint8_t offset = 0;

    /* check for proper number of arguments */
    if((nrhs != 6) && (nrhs != 7))
    {
        mexErrMsgIdAndTxt("CuteDSP:Filter:nargs", "Six or seven inputs required.");
        goto L_exit;
    }

    if(nlhs!=2)
    {
        mexErrMsgIdAndTxt("CuteDSP:Filter:nlhs", "Two outputs required.");
        goto L_exit;
    }

    /* Check filter type */
    if(!mxIsChar(prhs[offset]))
    { 
        mexErrMsgIdAndTxt("CuteDSP:Filter:FilterType", "Filter name must be a string.");
        goto L_exit;
    }
    else
    {
        #define FILTER_TYPE_NAME_SIZE (255)
        char filter_type_name[FILTER_TYPE_NAME_SIZE];
        if(0 != mxGetString(prhs[0], filter_type_name, FILTER_TYPE_NAME_SIZE))
        {
            mexErrMsgIdAndTxt("CuteDSP:Filter:FilterType", "Failed to get string with filter type name");
            goto L_exit;
        }

        if(strcmp(filter_type_name, "lowpass") == 0)
        {            
            filter_type = FILTER_C_TYPE_LP;
        }
        else
        {
            mexErrMsgIdAndTxt("CuteDSP:Filter:FilterType", "Unsupported filter type");
            goto L_exit;
        }

        if((FILTER_C_TYPE_LP == filter_type) || (FILTER_C_TYPE_HP == filter_type))
        {
            if(nrhs != 6)
            {
                mexErrMsgIdAndTxt("CuteDSP:Filter:nargs", "Six inputs required.");
                goto L_exit;
            }
        }
        else
        {
            if(nrhs != 7)
            {
                mexErrMsgIdAndTxt("CuteDSP:Filter:nargs", "Seven inputs required.");
                goto L_exit;
            }
        }
        offset++;
    }

    /* Check filter order */
    if(!mxIsDouble(prhs[offset]) || (mxGetNumberOfElements(prhs[offset]) != 1))
    {
        mexErrMsgIdAndTxt("CuteDSP:Filter:order", "Order argument is bad");
        goto L_exit;
    }
    else
    {
        order = mxGetScalar(prhs[offset]);
        offset++;
    }
    

    /* Sampling frequency */
    if(!mxIsDouble(prhs[offset]) || (mxGetNumberOfElements(prhs[offset]) != 1))
    {
        mexErrMsgIdAndTxt("CuteDSP:Filter:sampling_frequency", "Sampling frequency is bad");
        goto L_exit;
    }
    else
    {
        fs = mxGetScalar(prhs[offset]);
        offset++;
    }

    /* Filtering frequency 1 */
    if(!mxIsDouble(prhs[offset]) || (mxGetNumberOfElements(prhs[offset]) != 1))
    {
        mexErrMsgIdAndTxt("CuteDSP:Filter:filter1", "Filtering frequency 1 is bad");
        goto L_exit;
    }
    else
    {
        f1 = mxGetScalar(prhs[offset]);
        offset++;
    }

    /* Filtering frequency 2 */
    if(FILTER_C_TYPE_BP == filter_type)
    {
        if(!mxIsDouble(prhs[offset]) || (mxGetNumberOfElements(prhs[offset]) != 1))
        {
            mexErrMsgIdAndTxt("CuteDSP:Filter:filter2", "Filtering frequency 2 is bad");
            goto L_exit;
        }
        else
        {
            f2 = mxGetScalar(prhs[offset]);
            offset++;
        }
    }

    /* Inputs */
    if(mxGetNumberOfElements(prhs[offset]) != mxGetNumberOfElements(prhs[offset + 1]))
    {
        mexErrMsgIdAndTxt("CuteDSP:Filter:IQ", "Elements number is unaligned between I and Q components");
        goto L_exit;
    }
    len = mxGetNumberOfElements(prhs[offset]);
    if(order > len)
    {
        mexErrMsgIdAndTxt("CuteDSP:Filter:len", "Order is bigger than input data length");
        goto L_exit;
    }

    ii = mxGetPr(prhs[offset++]);
    iq = mxGetPr(prhs[offset++]);

    plhs[0] = mxCreateDoubleMatrix(1, len, mxREAL);
    plhs[1] = mxCreateDoubleMatrix(1, len, mxREAL);

    oi = mxGetPr(plhs[0]);
    oq = mxGetPr(plhs[1]);
    
    if(filterDbl(filter_type, order, fs, f1, ii, iq, oi, oq, len))
    {
        mexErrMsgIdAndTxt("CuteDSP:Filter:execution", "Failed to run filter");
    }

L_exit:
    return;
}