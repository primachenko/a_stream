#include "common.h"

#define MSG_LEN 128

int main(int argc, char const *argv[])
{
    printf("this is client\n");

    int rc;
    int fd;
    struct sockaddr target_addr;
    rc = open_socket_tx(&fd, &target_addr);
    if (0 != rc) return -1;
    char buf[MSG_LEN];
    int addr_len = sizeof(target_addr);

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        scanf("%s", buf);
        rc = (int) sendto(fd, (void *) buf, MSG_LEN, 0, (const struct sockaddr *) &target_addr, addr_len);
    }
    close(fd);
    return 0;
}