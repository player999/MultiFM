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
#ifndef _FM_RECEIVER_HPP_
#define _FM_RECEIVER_HPP_
#include "error.hpp"
#include "filter.hpp"
#include "mixer.hpp"
#include "audio_encoder.hpp"
#include <vector>
#include <string>
#if DEBUG_TRACE_DEMOD == 1
# include "tracer_tong.hpp"
#endif

namespace DSP
{
    /*! Radio station receiver with writing demodulated signal to sink */
    template <class T> class FmReceiver
    {
        public:
            /** @brief Constructor of FmReceiver class
             * @param[in] mp3_suffix prefix to file name
             * @param[in] fs sampling rate
             * @param[in] cf center frequency
             * @param[out] f frequency to receive. Must be in scope of bandwidth defined by sampling rate
             * @return It is a constructor. Nothing returned.
             * */
            FmReceiver(std::string mp3_suffix, double fs, double cf, double f);
            ~FmReceiver();
            /** @brief Processor of the piece of data
             * @param[in] buffer_i input I data
             * @param[in] buffer_q input Q data
             * @return Error code. #SUCCESS on success.
             * */
            Error processChunk(const std::vector<T> *buffer_i, const std::vector<T> *buffer_q);
        private:
            double fs;
            double relative_freq;
            AudioEncoder *encoder;
            LpFirFilter<T> *lowpass;
            LpFirFilter<T> *lowpass_audio;
            Mixer<T> *mixer;
            uint32_t decimator1;
            uint32_t decimator2;

            std::vector<T> deinter_i;
            std::vector<T> deinter_q;
            std::vector<T> mixed_i;
            std::vector<T> mixed_q;
            std::vector<T> filter1_i;
            std::vector<T> filter1_q;
            std::vector<T> omega;
            std::vector<T> omega_diff;
            std::vector<T> demodulated;
#if DEBUG_TRACE_DEMOD == 1
            TracerTong tracer;
#endif
    };
}
#endif /*_FM_RECEIVER_HPP_*/
