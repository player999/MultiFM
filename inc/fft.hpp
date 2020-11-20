#include <cstdint>
#include <cstdlib>
#include <vector>
#include "error.hpp"

namespace DSP
{
    template <class T> class FFT
    {
        public:
            FFT();
            Error transform2(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            Error inverse_transform2(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            Error transform_bluestein(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            Error inverse_transform_bluestein(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            Error transform(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
            Error inverse_transform(T *input_i, T *input_q, T *output_i, T *output_q, size_t length);
        private:
            Error updateW(size_t s, bool forward);
            void updateBitrev(size_t s);
            size_t singleBitrev(size_t idx, uint8_t w);
            std::vector<T> wi;
            std::vector<T> wq;
            std::vector<T> inverse_wi;
            std::vector<T> inverse_wq;
            std::vector<size_t> bitrev;
    };
}