#include "common.h"

int main(int argc, char const *argv[])
{
    int rc;
    int error;

    int fd;
    uint8_t buf[MSG_LEN];
    struct sockaddr target_addr;
    rc = open_socket_rx(&fd, &target_addr);
    if (0 != rc)
    {
        printf("open_socket_rx failed\n");
        return -1;
    }
    uint32_t addr_len = sizeof(target_addr);

    pa_simple * device = NULL;

    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };

    /* Create the recording stream */
    if (!(device = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        return -1;
    }

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        rc = (int) recvfrom(fd, buf, MSG_LEN, 0, (struct sockaddr *) &target_addr, &addr_len);
        if (!rc) break;

        if (pa_simple_write(device, buf, (size_t) rc, &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
            return -1;
        }
    }
    if (pa_simple_drain(device, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        return -1;
    }

    pa_simple_free(device);
    close(fd);
    return 0;
}