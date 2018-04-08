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

typedef struct
{
    int tx_fd;
    int rx_fd;
    int br_fd; // in progress
    struct pollfd pollfds;
    struct sockaddr addr;
    struct sockaddr br_addr; //in progress
    uint64_t byte_counter;
    uint16_t tx_port; //todo
    uint16_t rx_port; //todo
    uint16_t br_port; //todo
} net_t;

typedef struct
{
    pa_simple * playback;
    pa_simple * capture;
    pa_sample_spec sample_spec;
} sound_t;

typedef struct
{
    net_t net;
    sound_t sound;
    uint8_t mask_state;
    pthread_t handlers_tid;
} config_t;

#endif /* CONFIG_H */