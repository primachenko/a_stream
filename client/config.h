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
#include <pulse/simple.h>
#include <pulse/error.h>

typedef struct
{
    int fd;
    struct sockaddr addr;
    uint64_t byte_counter;
} net_t;

typedef struct
{
    pa_simple * device;
    pa_sample_spec sample_spec;
} sound_t;

typedef struct
{
    net_t net;
    sound_t sound;
    uint8_t mask_state;
} config_t;

#endif /* CONFIG_H */