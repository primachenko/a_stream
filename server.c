#include "common.h"

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

    FILE * file = fopen("test.wav", "r");
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fclose(file);
    file = fopen("copy.wav", "w");
    if (!file) return -2;
    int recvd = 0;
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        rc = (int) recvfrom(fd, buf, MSG_LEN, 0, (struct sockaddr *) &target_addr, &addr_len);
        if (!rc) break;
        printf("%d recv\n", recvd+=rc);
        fwrite(buf, MSG_LEN, 1, file);
        if (recvd > length) break;
    }
    printf("transmission done, length = %d\n", length);
    fflush(file);
    fclose(file);
    close(fd);
    return 0;
}