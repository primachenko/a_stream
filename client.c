#include "common.h"

int main(int argc, char const *argv[])
{
    printf("this is client\n");

    int rc;
    int error;

    int fd;
    uint8_t buf[MSG_LEN];
    struct sockaddr target_addr;
    rc = open_socket_tx(&fd, &target_addr);
    if (0 != rc)
    {
        printf("open_socket_tx failed\n");
        return -1;
    }
    int addr_len = sizeof(target_addr);
    
    pa_simple *device = NULL;

    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };

    /* Create the recording stream */
    if (!(device = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        return -1;
    }

    while (1)
    {
        if (pa_simple_read(device, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            break;
        }

        rc = (int) sendto(fd, (void *) buf, MSG_LEN, 0, (const struct sockaddr *) &target_addr, addr_len);
        if (!rc) break;

        memset(buf, 0, sizeof(buf));
    }

    pa_simple_free(device);
    close(fd);
    printf("done\n");
    return 0;
}