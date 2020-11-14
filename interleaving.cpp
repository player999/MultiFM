#include "interleaving.hpp"

using namespace DSP;

template <typename T, typename F> Error deinterleave(T *buffer, size_t total_elems, F *output_i, F *output_q)
{
    size_t offset;

    if(total_elems % 2)
    {
        return FAIL;
    }

    offset = 0;
    for(size_t ii = 0; ii < total_elems / 2; ii++)
    {
        output_i[ii] = (F)buffer[offset++];
        output_q[ii] = (F)buffer[offset++];
    }

    return SUCCESS;
}

Error deinterleave(uint8_t *buffer, size_t total_elems, double *output_i, double *output_q)
{
    return deinterleave<uint8_t, double>(buffer, total_elems, output_i, output_q);
}

Error deinterleave(uint8_t *buffer, size_t total_elems, float *output_i, float *output_q)
{
    return deinterleave<uint8_t, float>(buffer, total_elems, output_i, output_q);
}

Error deinterleave(double *buffer, size_t total_elems, double *output_i, double *output_q)
{
    return deinterleave<double, double>(buffer, total_elems, output_i, output_q);
}

Error deinterleave(float *buffer, size_t total_elems, float *output_i, float *output_q)
{
    return deinterleave<float, float>(buffer, total_elems, output_i, output_q);
}


template <typename T, typename F> Error deinterleave(std::vector<T> &buffer, F *output_i, F *output_q)
{
    size_t offset;

    if(buffer.size() % 2)
    {
        return FAIL;
    }

    offset = 0;
    for(size_t ii = 0; ii < buffer.size() / 2; ii++)
    {
        output_i[ii] = (F)buffer[offset++];
        output_q[ii] = (F)buffer[offset++];
    }

    return SUCCESS;
}

Error deinterleave(std::vector<uint8_t> &buffer, double *output_i, double *output_q)
{
    return deinterleave<uint8_t, double>(buffer, output_i, output_q);
}

Error deinterleave(std::vector<uint8_t> &buffer, float *output_i, float *output_q)
{
    return deinterleave<uint8_t, float>(buffer, output_i, output_q);
}

Error deinterleave(std::vector<double> &buffer, double *output_i, double *output_q)
{
    return deinterleave<double, double>(buffer, output_i, output_q);
}

Error deinterleave(std::vector<float> &buffer, float *output_i, float *output_q)
{
    return deinterleave<float, float>(buffer, output_i, output_q);
}

template <typename T, typename F> Error deinterleave(std::vector<T> &buffer, std::vector<F> &obuffer_i, std::vector<F> &obuffer_q)
{
    size_t offset;

    if(buffer.size() % 2)
    {
        return FAIL;
    }

    obuffer_i.resize(buffer.size() / 2);
    obuffer_q.resize(buffer.size() / 2);

    offset = 0;
    for(size_t ii = 0; ii < buffer.size() / 2; ii++)
    {
        obuffer_i[ii] = (F)buffer[offset++];
        obuffer_q[ii] = (F)buffer[offset++];
    }

    return SUCCESS;
}

Error deinterleave(std::vector<uint8_t> &buffer, std::vector<double> &obuffer_i, std::vector<double> &obuffer_q)
{
    return deinterleave<uint8_t, double>(buffer, obuffer_i, obuffer_q);
}

Error deinterleave(std::vector<uint8_t> &buffer, std::vector<float> &obuffer_i, std::vector<float> &obuffer_q)
{
    return deinterleave<uint8_t, float>(buffer, obuffer_i, obuffer_q);
}

Error deinterleave(std::vector<double> &buffer, std::vector<double> &obuffer_i, std::vector<double> &obuffer_q)
{
    return deinterleave<double, double>(buffer, obuffer_i, obuffer_q);
}

Error deinterleave(std::vector<float> &buffer, std::vector<float> &obuffer_i, std::vector<float> &obuffer_q)
{
    return deinterleave<float, float>(buffer, obuffer_i, obuffer_q);
}
