#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <pthread.h>

#include <pulse/simple.h>
#include <pulse/error.h>

#include <math.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>

typedef struct
{
    int tx_fd;
    int rx_fd;
    int br_fd; // in progress
    struct pollfd pollfds[2];
    struct sockaddr addr;
    struct sockaddr br_addr; //in progress
    uint64_t byte_counter;
    uint16_t tx_port; //todo
    uint16_t rx_port; //todo
    uint16_t br_port; //todo
    uint16_t br_port2; //todo
} net_t;

typedef struct
{
    pa_simple * playback;
    pa_simple * capture;
    pa_sample_spec sample_spec;
} sound_t;

typedef struct
{
    AVCodec        * codec;
    AVCodecContext * context;
    AVFrame        * frame;
    int              frame_size;
} codec_t;

typedef struct
{
    net_t net;
    sound_t sound;
    codec_t codec;
    uint8_t mask_state;
    pthread_t handlers_tid;
    pthread_t txthread_tid;
} config_t;

#endif /* CONFIG_H */