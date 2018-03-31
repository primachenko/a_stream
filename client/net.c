#include "net.h"

int open_socket(int * fd, struct sockaddr * addr)
{
    int rc;
    rc = socket(AF_INET, SOCK_DGRAM, 0);
    if (!rc) return -1;
    *fd = rc;

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = inet_addr(TX_ADDR);
    my_addr.sin_port = htons(TX_PORT);

    socklen_t addr_len = sizeof(my_addr);
    rc = bind(*fd, (const struct sockaddr *)&my_addr, addr_len);
    if (0 != rc) return -2;

    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_addr.s_addr = inet_addr(RX_ADDR);
    target_addr.sin_port = htons(RX_PORT);

    memcpy(addr, &target_addr, sizeof(target_addr));
    return 0;
}

int send_message(int fd, struct sockaddr * addr, uint8_t flags, void * data, uint16_t data_len)
{
    int rc;

    header_t header;
    memset(&header, 0, sizeof(header));

    header.length = data_len;
    header.flags = flags;

    uint16_t addr_len = sizeof(*addr);

    rc = sendto(fd, (void *) &header, sizeof(header), 0, (const struct sockaddr *) addr, (socklen_t) addr_len);
    if (!rc) return -1;

    if (header.length == 0) return 0;

    rc = sendto(fd, (void *) data, header.length, 0, (const struct sockaddr *) addr, (socklen_t) addr_len);
    if (!rc) return -1;

    return 0;
}