#ifndef CODEC_H
#define CODEC_H

#include "config.h"

#define PACKET_SIZE 1024

int codec_init(codec_t * codec);
int codec_encode_data(codec_t  * codec,
                      void     * input,
                      uint32_t   input_len,
                      void    ** output,
                      uint32_t * output_len);
int codec_deinit(codec_t * codec);

#endif /* CODEC_H */