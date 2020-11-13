#include "audio_encoder.hpp"
#include <exception>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

extern "C"
{
    #include "libavutil/channel_layout.h"
    #include "libavutil/common.h"
    #include "libavutil/frame.h"
    #include "libavutil/samplefmt.h"
}

using namespace DSP;

AudioEncoder::AudioEncoder(std::string fname)
{
    int ret;

    /* find the MP3 encoder */
    av_codec = avcodec_find_encoder(AV_CODEC_ID_MP3);
    if(!av_codec)
    {
        std::__throw_runtime_error("Did not find MP3 codec");
    }

    av_ctx = avcodec_alloc_context3(av_codec);

    /* put sample parameters */
    av_ctx->bit_rate = 64000;

    /* check that the encoder supports double input */
    av_ctx->sample_fmt = AV_SAMPLE_FMT_S16;
    
    /* select other audio parameters supported by the encoder */
    av_ctx->sample_rate    = 44100;
    av_ctx->channel_layout = AV_CH_LAYOUT_MONO;
    av_ctx->channels       = av_get_channel_layout_nb_channels(av_ctx->channel_layout);

    /* open it */
    if(avcodec_open2(av_ctx, av_codec, NULL) < 0)
    {
        std::__throw_runtime_error("Could not open codec");
    }

    coding_file = fopen(fname.c_str(), "wb");
    if(!coding_file)
    {
        std::__throw_runtime_error("Could not open file");
    }

    /* packet for holding encoded output */
    av_pkt = av_packet_alloc();
    if(!av_pkt)
    {
        std::__throw_runtime_error("Could not allocate the packet");
    }

    /* frame containing input raw audio */
    av_frame = av_frame_alloc();
    if(!av_frame)
    {
        std::__throw_runtime_error("Could not allocate the packet");
    }

    av_frame->nb_samples     = av_ctx->frame_size;
    av_frame->format         = av_ctx->sample_fmt;
    av_frame->channel_layout = av_ctx->channel_layout;

    /* allocate the data buffers */
    ret = av_frame_get_buffer(av_frame, 0);
    if(ret < 0)
    {
        std::__throw_runtime_error("Could not allocate audio data buffers");
    }
}

AudioEncoder::~AudioEncoder()
{
    int ret;

    /* flush the encoder */
    ret = avcodec_send_frame(av_ctx, NULL);
    if(ret >= 0)
    {
        while(ret >= 0)
        {
            ret = avcodec_receive_packet(av_ctx, av_pkt);
            if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            {
                break;
            }
            else if (ret < 0)
            {
                break;
            }

            fwrite(av_pkt->data, 1, av_pkt->size, coding_file);
            av_packet_unref(av_pkt);
        }
    }

    fclose(coding_file);

    if(av_frame)
    {
        av_frame_free(&av_frame);
    }

    if(av_pkt)
    {
        av_packet_free(&av_pkt);
    }

    if(av_ctx)
    {
        avcodec_free_context(&av_ctx);
    }
}

template <class T> Error AudioEncoder::encodeInternal(T *data, size_t len)
{
    int ret;
    size_t frame_count;
    int16_t *frame_data;
    T *source_data;

    frame_count = len / av_ctx->frame_size;
    if(len % av_ctx->frame_size)
    {
        frame_count++;
    }

    for(size_t ff = 0; ff < frame_count; ff++)
    {
        size_t transfer_size;
        ret = av_frame_make_writable(av_frame);
        if (ret < 0)
        {
            return Error::FAIL;
        }

        if((ff + 1) * av_ctx->frame_size > len)
        {
            transfer_size = len % av_ctx->frame_size;
        }
        else
        {
            transfer_size = av_ctx->frame_size;
        }

        frame_data = (int16_t *)av_frame->data[0];
        source_data = data + ff * av_ctx->frame_size;
        for(size_t ii = 0; ii < transfer_size; ii++)
        {
            frame_data[ii] = (int16_t)source_data[ii];
        }
        av_frame->nb_samples = transfer_size;

        ret = avcodec_send_frame(av_ctx, av_frame);
        if (ret < 0)
        {
            return Error::FAIL;
        }

        while (ret >= 0)
        {
            ret = avcodec_receive_packet(av_ctx, av_pkt);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            {
                break;
            }
            else if (ret < 0)
            {
                return Error::FAIL;
            }

            fwrite(av_pkt->data, 1, av_pkt->size, coding_file);
            av_packet_unref(av_pkt);
        }
    }

    return Error::SUCCESS;
}

Error AudioEncoder::encode(double *data, size_t len)
{
    return encodeInternal<double>(data, len);
}

Error AudioEncoder::encode(float *data, size_t len)
{
    return encodeInternal<float>(data, len);
}

extern "C"
{
    void audioCodeDbl(const char *fname, double *data, size_t len)
    {
        AudioEncoder ae(fname);
        Error err = ae.encode(data, len);
        if(SUCCESS != err)
        {
            printf("Failed!\n");
        }
    }

    void audioCodeFloat(const char *fname, float *data, size_t len)
    {
        AudioEncoder ae(fname);
        Error err = ae.encode(data, len);
        if(SUCCESS != err)
        {
            printf("Failed!\n");
        }
    }
}
