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

    rc = open_socket(&config->net.br_fd);
    if (0 != rc)
    {
        printf("open_socket br failed\n");
        return -1;
    }

    //-!- for test only
    config->net.tx_port = atoi(argv[1]);
    config->net.rx_port = atoi(argv[2]);
    config->net.br_port = atoi(argv[3]);

    //-!- todo chain
    fill_inet_sockaddr_rx(&config->net, RX_ADDR, config->net.tx_port);

    rc = bind_rx_fd(&config->net);
    if (0 != rc)
    {
        printf("bind_rx_fd failed\n");
        return -1;
    }

    fill_inet_sockaddr_br(&config->net, "255.255.255.255", config->net.br_port);

    rc = bind_br_fd(&config->net);
    if (0 != rc)
    {
        printf("bind_br_fd failed\n");
        return -1;
    }

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
        printf("pthread_create fds_handle failed\n");
        return -1;
    }

    rc = pthread_create(&config->txthread_tid, NULL, tx_pthread, (void *) config);
    if (0 != rc)
    {
        printf("pthread_create tx_pthread failed\n");
        return -1;
    }

    //-!-handle user input

    struct pollfd fds;
    fds.fd = 0;
    fds.events |= POLLIN;

    while (1)
    {
        rc = poll(&fds, 1, 1);
        if (rc == 0) continue;
        else if (!rc) break;
        else if (fds.revents & POLLIN)
        {
            rc = user_input_handle(config);
            if (0 != rc)
            {
                printf("user_input_handle failed\n");
                return -1;
            }
        }
    }

    rc = pthread_cancel(config->txthread_tid);
    if (0 != rc)
    {
        printf("pthread_cancel tx_pthread failed\n");
        return -1;
    }

    rc = pthread_cancel(config->handlers_tid);
    if (0 != rc)
    {
        printf("pthread_cancel fds_handle failed\n");
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