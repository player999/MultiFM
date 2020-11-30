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
#include "fm_receiver.hpp"
#include "diff.hpp"
#include "atan.hpp"
#include "mixer.hpp"
#include "filter.hpp"

using namespace std;
namespace DSP
{
    template <class T> FmReceiver<T>::FmReceiver(string mp3_suffix, double fs, double cf, double f)
    {
        relative_freq = f - cf;
        if(abs(relative_freq) > (fs/2))
        {
            __throw_runtime_error("Frequency is invalid");
        }
        this->fs = fs;
        decimator1 = (uint32_t)(fs / 400e3); //200e3 BW
        decimator2 = (fs/decimator1)/44100;

        encoder = new AudioEncoder(mp3_suffix + to_string((size_t)f) + ".mp3");
        lowpass = new LpFirFilter<T>(fs, 100e3, 96);
        lowpass_audio = new LpFirFilter<T>((fs/decimator1), 44100/2, 96);
        mixer = new Mixer<T>(fs, f, 1000000);

        mixed_i.reserve(1000000);
        mixed_q.reserve(1000000);
        filter1_i.reserve(1000000);
        filter1_q.reserve(1000000);
        omega.reserve(1000000);
        omega_diff.reserve(1000000);
        demodulated.reserve(1000000);
    }

    template <class T> FmReceiver<T>::~FmReceiver()
    {
        delete encoder;
        delete lowpass;
        delete lowpass_audio;
        delete mixer;
    }

    template <class T> Error FmReceiver<T>::processChunk(const vector<T> *buffer_i, const vector<T> *buffer_q)
    {
        Error err;

        /* [Im,Qm] = mixer_ml(Fs, station, I, Q); */
#if DEBUG_TRACE_DEMOD == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
        mixed_i.resize(buffer_i->size());
        mixed_q.resize(buffer_q->size());
        //err = mix(fs, relative_freq, buffer_i->data(), buffer_q->data(), mixed_i.data(), mixed_q.data(), buffer_i->size());
        err = mixer->mix(buffer_i->data(), buffer_q->data(), mixed_i.data(), mixed_q.data(), buffer_i->size());
        if(err != SUCCESS)
        {
            return err;
        }

        /* [If,Qf] = filter_ml('lowpass', 256, Fs, 100e3, decimator, Im, Qm); */
#if DEBUG_TRACE_DEMOD == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
        filter1_i.resize(mixed_i.size() / decimator1);
        filter1_q.resize(mixed_q.size() / decimator1);
        err = lowpass->executeCpxDecim(mixed_i.data(), mixed_q.data(), filter1_i.data(), filter1_q.data(), decimator1, mixed_i.size());
        if(err != SUCCESS)
        {
            return err;
        }

        /* omega = atan_ml(If,Qf); */
#if DEBUG_TRACE_DEMOD == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
        omega.resize(filter1_i.size());
        err = ataniq(filter1_i.data(), filter1_q.data(), omega.data(), filter1_i.size());
        if(err != SUCCESS)
        {
            return err;
        }

        /* y = diff_ml(omega); */
#if DEBUG_TRACE_DEMOD == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
        omega_diff.resize(omega.size());
        err = diffangle(omega.data(), omega_diff.data(), omega.size());
        if(err != SUCCESS)
        {
            return err;
        }

        /* [z1,~] = filter_ml('lowpass', 256, Fs/decimator, AFs/2, (Fs/decimator)/AFs, y, y); */
#if DEBUG_TRACE_DEMOD == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
        demodulated.resize(omega_diff.size() / decimator2);
        err = lowpass_audio->executeRealDecim(omega_diff.data(), demodulated.data(), decimator2, omega_diff.size());
        if(err != SUCCESS)
        {
            return err;
        }

        /* z1 = z1 .* 6000; */
#if DEBUG_TRACE_DEMOD == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
        for(size_t ii = 0; ii < demodulated.size(); ii++)
        {
            demodulated[ii] *= 6000;
        }

        /* audio_encoder_ml('shit.mp3', z1); */
#if DEBUG_TRACE_DEMOD == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif
        err = encoder->encode(demodulated.data(), demodulated.size());
        if(err != SUCCESS)
        {
            return err;
        }
#if DEBUG_TRACE_DEMOD == 1
        if(trace_timer) tracer.tick(__FILE__, __LINE__);
#endif

        return SUCCESS;
    }
    template class FmReceiver<double>;
    template class FmReceiver<float>;
}
