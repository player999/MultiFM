/******************************************************************************
Copyright 2020 Taras Zakharchenko taras.zakharchenko@gmail.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1.  Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
3.  Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/
#include <cstdint>
#include <iostream>
#include <gtest/gtest.h>
#include "filter.hpp"
#include "audio_encoder.hpp"
#include "fft.hpp"

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
}

TEST(Coding, AudioCoding)
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

TEST(FFT, point8)
{
#define FFT_SIZE (8)
    DSP::FFT<double> fft;
    double in_i[FFT_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};
    double in_q[FFT_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
    double out_i[FFT_SIZE];
    double out_q[FFT_SIZE];
    double exp_i[FFT_SIZE] = {36, -4, -4, -4, -4, -4, -4, -4};
    double exp_q[FFT_SIZE] = {0, 9.6569, 4.0000, 1.6569, 0, -1.6569, -4.0000, -9.6569};
    DSP::Error err;

    err = fft.transform2(in_i, in_q, out_i, out_q, FFT_SIZE);
    ASSERT_EQ(err, DSP::Error::SUCCESS) << "Bad return code";

    for(size_t ii = 0; ii < FFT_SIZE; ii++)
    {
        ASSERT_EQ(round(100*out_i[ii]), round(100*exp_i[ii])) << "Bad expected output value, index = " << ii;
        ASSERT_EQ(round(100*out_q[ii]), round(100*exp_q[ii])) << "Bad expected output value, index = " << ii;
    }
}

TEST(FFT, inverse_point8)
{
#define FFT_SIZE (8)
    DSP::FFT<double> fft;
    double in_i[FFT_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};
    double in_q[FFT_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
    double out_i[FFT_SIZE];
    double out_q[FFT_SIZE];
    double out2_i[FFT_SIZE];
    double out2_q[FFT_SIZE];
    DSP::Error err;

    err = fft.transform2(in_i, in_q, out_i, out_q, FFT_SIZE);
    ASSERT_EQ(err, DSP::Error::SUCCESS) << "Bad return code";
    err = fft.inverse_transform2(out_i, out_q, out2_i, out2_q, FFT_SIZE);
    ASSERT_EQ(err, DSP::Error::SUCCESS) << "Bad return code";

    for(size_t ii = 0; ii < FFT_SIZE; ii++)
    {
        ASSERT_EQ(round(100*in_i[ii]), round(100*out2_i[ii])) << "Bad expected output value, index = " << ii;
        ASSERT_EQ(round(100*in_q[ii]), round(100*out2_q[ii])) << "Bad expected output value, index = " << ii;
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}