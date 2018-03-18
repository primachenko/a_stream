#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RX_ADDR "127.0.0.1"
#define RX_PORT 32000
#define TX_ADDR "127.0.0.1"
#define TX_PORT 33000

int open_socket_rx(int * fd, struct sockaddr * addr);
int open_socket_tx(int * fd, struct sockaddr * addr);