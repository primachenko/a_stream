#include "messages.h"
#include "common.h"

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

    uint32_t sended = 0;
    FILE * file = fopen("test.wav", "r");
    fseek(file, 0, SEEK_END);
    uint32_t length = ftell(file); 
    printf("length = %d\n", length);
    rewind(file);
    while (0 == feof(file) || EOF != fread(buf, MSG_LEN, 1, file))
    {
        rc = (int) sendto(fd, (void *) buf, MSG_LEN, 0, (const struct sockaddr *) &target_addr, addr_len);
        if (!rc) break;
        // printf("%d sended\n", sended+=MSG_LEN);
        sended += rc;
        memset(buf, 0, sizeof(buf));
        usleep(500);
        if (sended > length) break;
    }
    rc = (int) sendto(fd, (void *) buf, MSG_LEN, 0, (const struct sockaddr *) &target_addr, addr_len);
    printf("done\n");
    close(fd);
    return 0;
}