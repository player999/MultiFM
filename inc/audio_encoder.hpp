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
#ifndef _AUDIO_ENCODER_HPP_
#define _AUDIO_ENCODER_HPP_

#include <string>
#include <cstdlib>
#include <error.hpp>

extern "C"
{
    #include <libavcodec/avcodec.h>
}

namespace DSP
{
    /*! Class for compressing demodulated audio to MP3 */
    class AudioEncoder
    {
        public:
            AudioEncoder(std::string fname);
            Error encode(double *data, size_t len);
            Error encode(float *data, size_t len);
            ~AudioEncoder();

        private:
            template <class T> Error encodeInternal(T *data, size_t len);
            AVCodecContext *av_ctx= NULL;
            const AVCodec *av_codec = NULL;
            FILE *coding_file = NULL;
            AVPacket *av_pkt = NULL;
            AVFrame *av_frame = NULL;
    };
};

#endif /*_AUDIO_ENCODER_HPP_*/