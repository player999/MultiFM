#ifndef _MIXER_H_
#define _MIXER_H_

#include <stdlib.h>

int mixDbl(double fs, double f, double *input_i, double *input_q, double *output_i, double *output_q, size_t len);
int mixFloat(float fs, float f, float *input_i, float *input_q, float *output_i, float *output_q, size_t len);

#endif /*_MIXER_H_*/