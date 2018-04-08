#include "handlers.h"

static int rx_message_handle(config_t * config, message_t * message)
{
    int rc;

    rc = sound_write_data(&config->sound, message->payload, message->header.length);
    if (0 != rc) return -1;
    return 0;
}

static int rx_fd_handle(config_t * config)
{
    int rc;
    message_t * data;

    rc = recv_message(&config->net, (void **) &data);;
    if (0 != rc) return -1;

    rc = rx_message_handle(config, data);
    if (0 != rc) return -1;

    return 0;
}

static int brd_message_handle(config_t * config, message_t * message)
{
    return 0;
}

static int brd_fd_handle(config_t * config)
{
    int rc;
    message_t * data;

    rc = recv_brd_message(&config->net, (void **) &data);;
    if (0 != rc) return -1;

    rc = brd_message_handle(config, data);
    if (0 != rc) return -1;

    return 0;
}

void * fds_handle(void * data)
{
    int rc;
    struct pollfd * fds;
    config_t * config;
    config = (config_t *) data;
    fds = &config->net.pollfds;
    while (1)
    {
        rc = poll(fds, 1, POLL_TIMEOUT);
        if (0 == rc)
            continue;
        else if (!rc)
            return NULL;
        else
        {
            if (fds->revents & POLLIN)
            {
                rc = rx_fd_handle(config);
                if (0 != rc)
                    return NULL;
            }
        }
    }

    return NULL;
}

int fds_init(net_t * net)
{
    net->pollfds.fd = net->rx_fd;
    net->pollfds.events |= POLLIN;

    return 0;
}