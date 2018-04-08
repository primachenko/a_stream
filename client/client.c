#include "config.h"
#include "net.h"
#include "sound.h"
#include "handlers.h"

int main(int argc, char const *argv[])
{
    int rc;

    config_t * config;
    config = malloc(sizeof(*config));
    if (!config)
    {
        printf("malloc failed\n");
        return -1;
    }

    memset(config, 0, sizeof(*config));

    rc = open_socket(&config->net.tx_fd);
    if (0 != rc)
    {
        printf("open_socket tx failed\n");
        return -1;
    }

    rc = open_socket(&config->net.rx_fd);
    if (0 != rc)
    {
        printf("open_socket rx failed\n");
        return -1;
    }

    //-!- todo chain
    fill_inet_sockaddr(&config->net, (char *) argv[1], RX_PORT);

    //-!- va_args
    rc = sound_init_sample_spec(&config->sound, 0, 0, 0);
    if (0 != rc)
    {
        printf("init_sample_spec failed\n");
        return -1;
    }

    rc = sound_create_capture_device(&config->sound);
    if (0 != rc)
    {
        printf("create_capture_device failed\n");
        return -1;
    }

    rc = sound_create_playback_device(&config->sound);
    if (0 != rc)
    {
        printf("sound_create_playback_device failed\n");
        return -1;
    }

    rc = fds_init(&config->net);
    if (0 != rc)
    {
        printf("fds_init failed\n");
        return -1;
    }

    rc = pthread_create(&config->handlers_tid, NULL, fds_handle, (void *) config);
    if (0 != rc)
    {
        printf("pthread_create failed\n");
        return -1;
    }

    header_t header;
    memset(&header, 0, sizeof(header));

    uint8_t buf[MAX_PAYLOAD_LEN];
    config->mask_state |= 0x000001;

    struct pollfd fds;
    memset(&fds, 0, sizeof(fds));
    fds.fd = STDIN_FILENO;
    fds.events |= POLLIN;

    while (config->mask_state & 0x000001)
    {
        rc = sound_read_data(&config->sound, buf, MAX_PAYLOAD_LEN);
        if (0 != rc) break;

        rc = send_message(&config->net, message_flag_new, buf, MAX_PAYLOAD_LEN);
        if (0 != rc) break;

        memset(buf, 0, sizeof(buf));

        rc = poll(&fds, 1, 1);
        if (rc > 0 && fds.revents & POLLIN)
        {
            printf("exit\n");
            if ('s' == getchar()) config->mask_state &= 0x000000;
        }
    }

    rc = pthread_cancel(config->handlers_tid);
    if (0 != rc)
    {
        printf("pthread_cancel failed\n");
        return -1;
    }

    rc = sound_destroy_capture_sound_device(&config->sound);
    if (0 != rc)
    {
        printf("destroy_sound_device failed\n");
        return -1;
    }

    rc = close_socket(&config->net.tx_fd);
    if (0 != rc)
    {
        printf("close_socket tx failed\n");
        return -1;
    }

    rc = close_socket(&config->net.rx_fd);
    if (0 != rc)
    {
        printf("close_socket rx failed\n");
        return -1;
    }

    free(config);

    return 0;
}