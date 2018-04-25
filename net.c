#include "net.h"

static inline int net_open_socket(net_t * net)
{
    if (!net) return -1;

    int rc = socket(AF_INET, SOCK_DGRAM, 0);
    if (!rc) return -1;

    net->fd = rc;
    return 0;
}

static inline int net_close_socket(net_t * net)
{
    if (!net) return -1;

    if (net->fd)
        close(net->fd);
    else
        return -1;

    return 0;
}

static inline int net_fill_addr(net_t * net, char * host, uint16_t port)
{
    if (!net)  return -1;
    if (!host) return -1;
    if (!port) return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host);
    addr.sin_port = htons(port);

    memcpy(&net->addr, &addr, sizeof(addr));

    return 0;
}

static inline int net_clear_addr(net_t * net)
{
    if (!net) return -1;

    memset(&net->addr, 0, sizeof(net->addr));

    return 0;
}


int net_initialize(net_t ** net, char * host, uint16_t port)
{
    if (!net)  return -1;
    if (!host) return -1;
    if (!port) return -1;

    *net = malloc(sizeof(net_t));
    if (!*net) return -1;

    int rc = net_fill_addr(*net, host, port);
    if (0 != rc) return -1;

    rc = net_open_socket(*net);
    if (0 != rc) return -1;

    sprintf((*net)->socket, "%s:%d", host, port);

    return 0;
}

int net_finish(net_t * net)
{
    if (!net)  return -1;

    int rc = net_close_socket(net);
    if (0 != rc) return -1;

    rc = net_clear_addr(net);
    if (0 != rc) return -1;

    free(net);

    return 0;
}

int net_send_message(net_t * net, message_t * message_in)
{
    if (!net) return -1;
    if (!message_in) return -1;

    int rc;
    header_t * header = (header_t *) &message_in->header;
    uint16_t addr_len = sizeof(net->addr);

    rc = sendto(net->fd, (void *) header, sizeof(*header), 0, &net->addr, (socklen_t) addr_len);
    if (!rc) return -1;

    if (header->length == 0) return 0;

    rc = sendto(net->fd, (void *) &message_in->payload, header->length, 0, &net->addr, (socklen_t) addr_len);
    if (!rc) return -1;

    return 0;
}

int net_recv_message(net_t * net, message_t ** message_out)
{
    if (!net) return -1;
    if (!message_out) return -1;

    int rc;

    message_t * message;
    message = (message_t *) malloc(sizeof(header_t));
    if (!message) return -1;
    memset(message, 0, sizeof(*message));
    header_t * header = (header_t *) &message->header;
    uint16_t addr_len = sizeof(net->addr);

    rc = recvfrom(net->fd, (void *) header, sizeof(*header), 0, &net->addr, (socklen_t *) &addr_len);
    if (!rc) return -1;

    if (header->length == 0) return 0;

    message = (message_t *) realloc(message, sizeof(*header) + header->length);
    if (!message)
    {
        free(header);
        return -1;
    }

    rc = recvfrom(net->fd, &message->payload, header->length, 0, &net->addr, (socklen_t *) &addr_len);
    if (!rc) return -1;

    *message_out = message;

    return 0;
}