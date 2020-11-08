#ifndef _FILTER_H_
#define _FILTER_H_

#define FILTER_C_TYPE_LP (0)
#define FILTER_C_TYPE_HP (1)
#define FILTER_C_TYPE_BP (2)

#include <stdint.h>
#include <stdlib.h>

int filterDbl(uint8_t ftype, uint32_t order, double fs, double f, double *ii, double *iq, double *oi, double *oq, uint32_t decim, size_t len);

#endif /*_FILTER_H_*/