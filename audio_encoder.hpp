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