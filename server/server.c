#include "net.h"

int main(int argc, char const *argv[])
{
    int rc;
    int error;

    int fd;
    // uint8_t buf[MSG_LEN];
    struct sockaddr addr;
    rc = open_socket(&fd, &addr);
    if (0 != rc)
    {
        printf("open_socket failed\n");
        return -1;
    }

    pa_simple * device = NULL;

    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };

    if (!(device = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        return -1;
    }

    while (1)
    {
        void * data = NULL;
        rc = recv_message(fd, &addr, &data);;
        if (0 != rc) break;

        message_t * message;
        message = (message_t *) data;

        if (pa_simple_write(device, message->payload, (size_t) message->header.length, &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
            return -1;
        }
        free(data);
    }
    if (pa_simple_drain(device, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        return -1;
    }

    pa_simple_free(device);
    close(fd);
    return 0;
}