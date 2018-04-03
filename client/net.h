#ifndef NET_H
#define NET_H

#include "config.h"
#include "messages.h"

#define RX_ADDR "192.168.0.105"
#define RX_PORT 32000
#define TX_ADDR "192.168.0.105"
#define TX_PORT 33000

int open_socket(net_t * net);
int close_socket(net_t * net);
int send_message(net_t * net, uint8_t flags, void * data, uint16_t data_len);

#endif /* NET_H */