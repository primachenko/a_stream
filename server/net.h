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

#define RX_ADDR "127.0.0.1"
#define RX_PORT 32000
#define TX_ADDR "127.0.0.1"
#define TX_PORT 33000

int open_socket(int * fd, struct sockaddr * addr);
int recv_message(int fd, struct sockaddr * addr, void ** data);

#endif /* NET_H */