#include "codec.h"

int codec_init
(
    codec_t * codec
)
{
    if (!codec) return -1;

    AVCodec * av_codec;
    AVCodecContext * context;
    AVFrame * frame;

    avcodec_register_all();

    av_codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!av_codec) return -1;

    context = avcodec_alloc_context3(av_codec);
    if (!context) return -1;
    context->bit_rate = 44100;
    context->sample_fmt = AV_SAMPLE_FMT_S16;
    context->sample_rate    = 44100;
    context->channel_layout = 0;
    context->channels       = 2;

    int rc;
    rc = avcodec_open2(context, av_codec, NULL);
    if (0 > rc) return -1;

    frame = av_frame_alloc();
    if (!frame) return -1;

    frame->nb_samples     = context->frame_size;
    frame->format         = context->sample_fmt;
    frame->channel_layout = context->channel_layout;

    int frame_size;
    frame_size = av_samples_get_buffer_size(NULL, context->channels, context->frame_size, context->sample_fmt, 0);
    if (!frame_size) return -1;

    rc = avcodec_fill_audio_frame(frame, context->channels, context->sample_fmt, NULL, frame_size, 0);
    if (rc < 0) return -1;

    codec->codec = av_codec;
    codec->context = context;
    codec->frame = frame;
    codec->frame_size = frame_size;

    return 0;
}

int codec_encode_data(codec_t  * codec,
                      void     * input,
                      uint32_t   input_len,
                      void    ** output,
                      uint32_t * output_len)
{
    if (!codec)      return -1;
    if (!input)      return -1;
    if (!input_len)  return -1;
    if (!output)     return -1;
    if (!output_len) return -1;

    AVCodecContext * context = codec->context;
    AVFrame * frame = codec->frame;
    AVPacket packet;
    int got_output;
    int frame_size = codec->frame_size;
    int rc;
    int i;
    int result_size;
    int input_ptr;

    for (result_size = 0, input_ptr = 0; input_ptr < input_len; input_ptr += frame_size)
    {
        av_init_packet(&packet);
        packet.data = NULL;
        packet.size = 0;

        rc = avcodec_fill_audio_frame(frame, context->channels, context->sample_fmt, (const uint8_t *) input + input_ptr, frame_size, 0);
        if (rc < 0) return -1;

        rc = avcodec_encode_audio2(context, &packet, frame, &got_output);
        if (rc < 0) return -1;

        if (got_output)
        {
            *output = realloc(*output, result_size + packet.size);
            if (!*output) return -1;

            memcpy(*output + result_size, packet.data, packet.size);
            result_size += packet.size;
            av_free_packet(&packet);
        }
    }

    for (got_output = 1; got_output; i++)
    {
        rc = avcodec_encode_audio2(context, &packet, NULL, &got_output);
        if (rc < 0) return -1;

        if (got_output)
        {
            *output = realloc(*output, result_size + packet.size);
            if (!*output) return -1;

            memcpy(*output + result_size, packet.data, packet.size);
            result_size += packet.size;
            av_free_packet(&packet);
        }
    }

    *output_len = result_size;
    return 0;
}
int codec_deinit
(
    codec_t * codec
)
{
    if (!codec) return -1;

    av_frame_free(&codec->frame);
    avcodec_close(codec->context);
    av_free(codec->context);

    return 0;
}