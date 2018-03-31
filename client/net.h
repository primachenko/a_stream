#ifndef NET_H
#define NET_H

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
#include <sys/sendfile.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#include "messages.h"

#define RX_ADDR "192.168.0.103"
#define RX_PORT 32000
#define TX_ADDR "192.168.0.101"
#define TX_PORT 33000

int open_socket(int * fd, struct sockaddr * addr);
int send_message(int fd, struct sockaddr * addr, uint8_t flags, void * data, uint16_t data_len);

#endif /* NET_H */