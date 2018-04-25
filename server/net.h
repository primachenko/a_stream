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

#define MAX_PAYLOAD_LEN 1024
enum message_flags
{
	message_flag_min = 0,
	message_flag_new,
	message_flag_more,
	message_flag_end,
	message_flag_last = message_flag_end,
	message_flag_max = 255
};

typedef struct __attribute__((packed))
{
	uint32_t length;
	uint8_t flags;
} header_t;

typedef struct __attribute__((packed))
{
	header_t header;
	uint8_t payload[0];
} message_t;

#define PORT 32000

int open_socket(int * fd);
int recv_message(int fd, void ** data);

#endif /* NET_H */