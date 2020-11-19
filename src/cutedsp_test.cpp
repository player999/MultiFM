#include <cstdint>
#include <iostream>
#include <gtest/gtest.h>
#include "filter.hpp"
#include "audio_encoder.hpp"

TEST(FilterTest, Convolution3) {
    DSP::Error err;
    DSP::LpFirFilter<float> lp(10.0f, 5.0f, 3);
    float out_i[10], out_q[10];
    float in_i[10], in_q[10];
    memset(in_i, 0, sizeof(in_i));
    memset(in_q, 0, sizeof(in_q));
    memset(out_i, 0, sizeof(out_i));
    memset(out_q, 0, sizeof(out_q));

    lp.coefficients[0] = 2.0f;
    lp.coefficients[1] = 3.0f;
    lp.coefficients[2] = 4.0f;
    in_i[0] = 1.0f; in_q[0] = 1.0f;
    in_i[1] = 1.0f; in_q[1] = 1.0f;
    in_i[2] = 1.0f; in_q[2] = 1.0f;
    in_i[3] = 1.0f; in_q[3] = 1.0f;
    in_i[4] = 1.0f; in_q[4] = 1.0f;
    in_i[5] = 1.0f; in_q[5] = 1.0f;
    in_i[6] = 1.0f; in_q[6] = 1.0f;
    in_i[7] = 1.0f; in_q[7] = 1.0f;
    in_i[8] = 1.0f; in_q[8] = 1.0f;
    in_i[9] = 1.0f; in_q[9] = 1.0f;

    err = lp.executeCpx(in_i, in_q, out_i, out_q, 10);
    ASSERT_EQ(err, DSP::Error::SUCCESS);

    std::cout.precision(std::numeric_limits<float>::max_digits10 - 1);
    for(uint32_t ii = 0; ii < 10; ii++)
    {
        std::cout << std::scientific << out_i[ii] << " + i*" << out_q[ii] << std::endl;
    }
}

TEST(FilterTest, Convolution4) {
    DSP::Error err;
    DSP::LpFirFilter<float> lp(10.0f, 5.0f, 4);
    float out_i[10], out_q[10];
    float in_i[10], in_q[10];
    memset(in_i, 0, sizeof(in_i));
    memset(in_q, 0, sizeof(in_q));
    memset(out_i, 0, sizeof(out_i));
    memset(out_q, 0, sizeof(out_q));

    lp.coefficients[0] = 2.0f;
    lp.coefficients[1] = 3.0f;
    lp.coefficients[2] = 6.0f;
    lp.coefficients[3] = 7.0f;
    in_i[0] = 1.0f; in_q[0] = 1.0f;
    in_i[1] = 1.0f; in_q[1] = 1.0f;
    in_i[2] = 1.0f; in_q[2] = 1.0f;
    in_i[3] = 1.0f; in_q[3] = 1.0f;
    in_i[4] = 1.0f; in_q[4] = 1.0f;
    in_i[5] = 1.0f; in_q[5] = 1.0f;
    in_i[6] = 1.0f; in_q[6] = 1.0f;
    in_i[7] = 1.0f; in_q[7] = 1.0f;
    in_i[8] = 1.0f; in_q[8] = 1.0f;
    in_i[9] = 1.0f; in_q[9] = 1.0f;

    err = lp.executeCpx(in_i, in_q, out_i, out_q, 10);
    ASSERT_EQ(err, DSP::Error::SUCCESS);

    std::cout.precision(std::numeric_limits<float>::max_digits10 - 1);
    for(uint32_t ii = 0; ii < 10; ii++)
    {
        std::cout << std::scientific << out_i[ii] << " + i*" << out_q[ii] << std::endl;
    }
}

TEST(FilterTest, Decimation) {
    DSP::Error err;
    DSP::LpFirFilter<float> lp(10.0f, 5.0f, 3);
    float out_i[10], out_q[10];
    float in_i[10], in_q[10];
    memset(in_i, 0, sizeof(in_i));
    memset(in_q, 0, sizeof(in_q));
    memset(out_i, 0, sizeof(out_i));
    memset(out_q, 0, sizeof(out_q));

    lp.coefficients[0] = 2.0f;
    lp.coefficients[1] = 3.0f;
    lp.coefficients[2] = 4.0f;
    in_i[0] = 1.0f; in_q[0] = 1.0f;
    in_i[1] = 1.0f; in_q[1] = 1.0f;
    in_i[2] = 1.0f; in_q[2] = 1.0f;
    in_i[3] = 1.0f; in_q[3] = 1.0f;
    in_i[4] = 1.0f; in_q[4] = 1.0f;
    in_i[5] = 1.0f; in_q[5] = 1.0f;
    in_i[6] = 1.0f; in_q[6] = 1.0f;
    in_i[7] = 1.0f; in_q[7] = 1.0f;
    in_i[8] = 1.0f; in_q[8] = 1.0f;
    in_i[9] = 1.0f; in_q[9] = 1.0f;

    err = lp.executeCpxDecim(in_i, in_q, out_i, out_q, 2, 10);
    ASSERT_EQ(err, DSP::Error::SUCCESS);

    std::cout.precision(std::numeric_limits<float>::max_digits10 - 1);
    for(uint32_t ii = 0; ii < 10; ii++)
    {
        std::cout << std::scientific << out_i[ii] << " + i*" << out_q[ii] << std::endl;
    }
}

TEST(FilterTest, ConvolutionReal3) {
    DSP::Error err;
    DSP::LpFirFilter<float> lp(10.0f, 5.0f, 3);
    float out[10];
    float in[10];
    memset(in, 0, sizeof(in));
    memset(out, 0, sizeof(out));

    lp.coefficients[0] = 2.0f;
    lp.coefficients[1] = 3.0f;
    lp.coefficients[2] = 4.0f;
    in[0] = 1.0f;
    in[1] = 1.0f;
    in[2] = 1.0f;
    in[3] = 1.0f;
    in[4] = 1.0f;
    in[5] = 1.0f;
    in[6] = 1.0f;
    in[7] = 1.0f;
    in[8] = 1.0f;
    in[9] = 1.0f;

    err = lp.executeReal(in, out, 10);
    ASSERT_EQ(err, DSP::Error::SUCCESS);

    std::cout.precision(std::numeric_limits<float>::max_digits10 - 1);
    for(uint32_t ii = 0; ii < 10; ii++)
    {
        std::cout << std::scientific << out[ii] << std::endl;
    }
}

TEST(FilterTest, AudioCoding)
{
#define ELEMS (44100*5)
    DSP::AudioEncoder ae("test.mp3");
    double sine[ELEMS];
    double t, tincr;

    t = 0;
    tincr = 2 * M_PI * 440.0 / 44100;
    for(size_t ii = 0; ii < ELEMS; ii++)
    {
        sine[ii] = (int16_t)(sin(t) * 10000);
        t += tincr;
        if(t > (2 * M_PI))
        {
            t -= 2 * M_PI;
        }
    }
    DSP::Error err = ae.encode(sine, ELEMS);
    ASSERT_EQ(err, DSP::Error::SUCCESS) << "Bad return code";
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}