#include "net.h"

int open_socket(int * fd)
{
    int rc;
    rc = socket(AF_INET, SOCK_DGRAM, 0);
    if (!rc) return -1;

    *fd = rc;

    return 0;
}

int close_socket(int * fd)
{
    if (*fd)
    {
        close(*fd);
        *fd = -1;
    }
    else
    {
        return -1;
    }

    return 0;
}

static void fill_sockaddr(struct sockaddr_in * addr, int family, char * host, uint16_t port)
{
    addr->sin_family = family;
    addr->sin_addr.s_addr = inet_addr(host);
    addr->sin_port = htons(port);
}

int bind_rx_fd(net_t * net)
{
    struct sockaddr_in addr;
    fill_sockaddr(&addr, AF_INET, RX_ADDR, net->rx_port);

    int rc;
    rc = bind(net->rx_fd, (const struct sockaddr *) &addr, sizeof(addr));
    if (rc != 0)
    {
        return -1;
    }

    return 0;
}

int bind_br_fd(net_t * net)
{
    struct sockaddr_in addr;
    fill_sockaddr(&addr, AF_INET, "255.255.255.255", net->br_port);

    int rc;
    rc = bind(net->br_fd, (const struct sockaddr *) &addr, sizeof(addr));
    if (rc != 0)
    {
        return -1;
    }

    return 0;
}

void fill_inet_sockaddr_rx(net_t * net, char * host, uint16_t port)
{
    fill_sockaddr((struct sockaddr_in *) &net->addr, AF_INET, host, port);;
}

void fill_inet_sockaddr_br(net_t * net, char * host, uint16_t port)
{
    fill_sockaddr((struct sockaddr_in *) &net->br_addr, AF_INET, host, port);;
}


int send_message(net_t * net, uint8_t flags, void * data, uint16_t data_len)
{
    int rc;

    header_t header;
    memset(&header, 0, sizeof(header));

    header.length = data_len;
    header.flags = flags;

    uint16_t addr_len = sizeof(net->addr);

    rc = sendto(net->tx_fd, (void *) &header, sizeof(header), 0, &net->addr, (socklen_t) addr_len);
    if (!rc) return -1;
    net->byte_counter += sizeof(header);

    if (header.length == 0) return 0;

    rc = sendto(net->tx_fd, (void *) data, header.length, 0, &net->addr, (socklen_t) addr_len);
    if (!rc) return -1;

    net->byte_counter += header.length;

    return 0;
}

int recv_message(net_t * net, void ** data)
{
    int rc;

    header_t header;
    memset(&header, 0, sizeof(header));

    uint16_t addr_len = sizeof(net->addr);

    rc = recvfrom(net->rx_fd, (void *) &header, sizeof(header_t), 0, &net->addr, (socklen_t *) &addr_len);
    if (!rc) return -1;

    if (header.length == 0) return 0;

    uint8_t * payload = (uint8_t *) malloc(sizeof(message_t) + header.length);
    if (payload == NULL) return -1;

    *data = payload;
    memcpy((void *) payload, (void *) &header, sizeof(header));
    payload += sizeof(header);
    rc = recvfrom(net->rx_fd, payload, header.length, 0, &net->addr, (socklen_t *) &addr_len);
    if (!rc) return -1;


    return 0;
}

int send_brd_message(net_t * net, uint8_t flags, void * data, uint16_t data_len)
{
    int rc;

    header_t header;
    memset(&header, 0, sizeof(header));

    header.length = data_len;
    header.flags = flags;

    uint16_t addr_len = sizeof(net->addr);

    rc = sendto(net->br_fd, (void *) &header, sizeof(header), 0, &net->addr, (socklen_t) addr_len);
    if (!rc) return -1;
    net->byte_counter += sizeof(header);

    if (header.length == 0) return 0;

    rc = sendto(net->br_fd, (void *) data, header.length, 0, &net->addr, (socklen_t) addr_len);
    if (!rc) return -1;

    net->byte_counter += header.length;

    return 0;
}

int recv_brd_message(net_t * net, void ** data)
{
    int rc;

    header_t header;
    memset(&header, 0, sizeof(header));

    uint16_t addr_len = sizeof(net->addr);

    rc = recvfrom(net->br_fd, (void *) &header, sizeof(header_t), 0, &net->br_addr, (socklen_t *) &addr_len);
    if (!rc) return -1;

    if (header.length == 0) return 0;

    uint8_t * payload = (uint8_t *) malloc(sizeof(message_t) + header.length);
    if (payload == NULL) return -1;

    *data = payload;
    memcpy((void *) payload, (void *) &header, sizeof(header));
    payload += sizeof(header);
    rc = recvfrom(net->br_fd, payload, header.length, 0, &net->addr, (socklen_t *) &addr_len);
    if (!rc) return -1;


    return 0;
}