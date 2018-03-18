#include "common.h"

#define MSG_LEN 128

int main(int argc, char const *argv[])
{
    printf("this is server\n");

    int rc;
    int fd;
    struct sockaddr target_addr;
    rc = open_socket_rx(&fd, &target_addr);
    if (0 != rc) return -1;

    char buf[MSG_LEN];
    uint32_t addr_len = sizeof(target_addr);

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        rc = (int) recvfrom(fd, buf, MSG_LEN, 0, (struct sockaddr *) &target_addr, &addr_len);
        printf("%s\n", buf);
    }
    close(fd);
    return 0;
}