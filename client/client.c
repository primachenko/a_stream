#include "net.h"

int main(int argc, char const *argv[])
{
    int rc;
    // int error;

    int fd;
    // uint8_t buf[MSG_LEN];
    struct sockaddr addr;
    rc = open_socket(&fd, &addr);
    if (0 != rc)
    {
        printf("open_socket failed\n");
        return -1;
    }

    // pa_simple * device = NULL;

    // static const pa_sample_spec ss = {
    //     .format = PA_SAMPLE_S16LE,
    //     .rate = 44100,
    //     .channels = 2
    // };

    // if (!(device = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
    //     fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
    //     return -1;
    // }

    header_t header;
    memset(&header, 0, sizeof(header));

    while (1)
    {
        void * data = NULL;
        rc = send_message(fd, &addr, message_flag_new, data, 0);
        if (0 != rc) break;

        // if (pa_simple_read(device, buf, sizeof(buf), &error) < 0) {
        //     fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
        //     break;
        // }

        // rc = sendto(fd, (void *) buf, MSG_LEN, 0, (const struct sockaddr *) &addr, addr_len);
        // if (!rc) break;

        // memset(buf, 0, sizeof(buf));
    }

    // pa_simple_free(device);
    close(fd);
    return 0;
}