#include "net.h"

int open_socket(int * fd)
{
    int rc;
    rc = socket(AF_INET, SOCK_DGRAM, 0);
    if (!rc) return -1;
    *fd = rc;

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(PORT);

    socklen_t addr_len = sizeof(my_addr);
    rc = bind(*fd, (const struct sockaddr *)&my_addr, addr_len);
    if (0 != rc) return -2;
    return 0;
}

int recv_message(int fd, void ** data)
{
    int rc;

    header_t header;
    memset(&header, 0, sizeof(header));

    rc = recv(fd, (void *) &header, sizeof(header_t), 0);
    if (!rc) return -1;

    if (header.length == 0) return 0;

    uint8_t * payload = (uint8_t *) malloc(sizeof(message_t) + header.length);
    if (payload == NULL) return -1;

    *data = payload;
    memcpy((void *) payload, (void *) &header, sizeof(header));
    payload += sizeof(header);
    rc = recv(fd, payload, header.length, 0);
    if (!rc) return -1;


    return 0;
}