#ifndef NET_H
#define NET_H

#include "config.h"
#include "messages.h"

#define RX_ADDR "127.0.0.1"
#define RX_PORT 32000
#define TX_ADDR "127.0.0.1"
#define TX_PORT 33000

int open_socket(int * fd);
int close_socket(int * fd);
void fill_inet_sockaddr(net_t * net, char * host, uint16_t port);
int send_message(net_t * net, uint8_t flags, void * data, uint16_t data_len);
int recv_message(net_t * net, void ** data);

#endif /* NET_H */