#ifndef NET_H
#define NET_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ADDR            "127.0.0.1"
#define MAX_PAYLOAD_LEN 1024
#define PORT            32000
#define SOCKET_LEN      22

typedef struct __attribute__((packed))
{
    uint32_t length;
} header_t;

typedef struct __attribute__((packed))
{
    header_t header;
    uint8_t payload[0];
} message_t;

typedef struct
{
    int fd;
    struct sockaddr addr;
    char socket[SOCKET_LEN];
} net_t;

typedef enum
{
    CONNECTION_EST,
    CONNECTION_NO
} connection_status_t;

int net_initialize(net_t ** net, char * host, uint16_t port);
int net_finish(net_t * net);

int net_send_message(net_t * net, message_t * message_in);
int net_recv_message(net_t * net, message_t ** message_out);

#endif /* NET_H */