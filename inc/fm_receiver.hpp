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
    template <class T> class FmReceiver
    {
        public:
            FmReceiver(std::string mp3_suffix, double fs, double cf, double f);
            ~FmReceiver();
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
