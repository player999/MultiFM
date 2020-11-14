#ifndef _INTERLEAVING_HPP_
#define _INTERLEAVING_HPP_

#include "error.hpp"
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace DSP
{
    Error deinterleave(uint8_t *buffer, size_t total_elems, double *output_i, double *output_q);
    Error deinterleave(uint8_t *buffer, size_t total_elems, float *output_i, float *output_q);
    Error deinterleave(double *buffer, size_t total_elems, double *output_i, double *output_q);
    Error deinterleave(float *buffer, size_t total_elems, float *output_i, float *output_q);

    Error deinterleave(std::vector<uint8_t> &buffer, double *output_i, double *output_q);
    Error deinterleave(std::vector<uint8_t> &buffer, float *output_i, float *output_q);
    Error deinterleave(std::vector<double> &buffer, double *output_i, double *output_q);
    Error deinterleave(std::vector<float> &buffer, float *output_i, float *output_q);

    Error deinterleave(std::vector<uint8_t> &buffer, std::vector<double> &obuffer_i, std::vector<double> &obuffer_q);    
    Error deinterleave(std::vector<uint8_t> &buffer, std::vector<float> &obuffer_i, std::vector<float> &obuffer_q);
    Error deinterleave(std::vector<double> &buffer, std::vector<double> &obuffer_i, std::vector<double> &obuffer_q);    
    Error deinterleave(std::vector<float> &buffer, std::vector<float> &obuffer_i, std::vector<float> &obuffer_q);
}

#endif /*_INTERLEAVING_HPP_*/