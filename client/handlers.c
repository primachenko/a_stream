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
    printf("%s\n", message->payload);
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
    fds = config->net.pollfds;
    while (1)
    {
        rc = poll(fds, 2, POLL_TIMEOUT);
        if (0 == rc)
            continue;
        else if (!rc)
            return NULL;
        else
        {
            if (fds[0].revents & POLLIN)
            {
                rc = rx_fd_handle(config);
                if (0 != rc)
                    return NULL;
            }
            if (fds[1].revents & POLLIN)
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
    net->pollfds[0].fd = net->rx_fd;
    net->pollfds[0].events |= POLLIN;

    net->pollfds[1].fd = net->br_fd;
    net->pollfds[1].events |= POLLIN;

    return 0;
}

void * tx_pthread(void * data)
{
    config_t * config;
    config = (config_t *) data;

    int rc;
    header_t header;
    memset(&header, 0, sizeof(header));

    uint8_t buf[MAX_PAYLOAD_LEN];
    // config->mask_state |= 0x000001;

    void * codec_out = NULL;
    uint32_t codec_out_len = 0;

    while (1)
    {
        if (!config->mask_state & 0x000001)
        {
            sleep(1);
            continue;
        }
        rc = sound_read_data(&config->sound, buf, MAX_PAYLOAD_LEN);
        if (0 != rc) break;


        rc = codec_encode_data(&config->codec, (void *) buf, MAX_PAYLOAD_LEN, &codec_out, &codec_out_len);
        if (0 != rc) break;

        rc = send_message(&config->net, message_flag_new, codec_out, codec_out_len);
        if (0 != rc) break;

        if (codec_out) free(codec_out);

        memset(buf, 0, sizeof(buf));
    }

    return NULL;
}

int user_input_handle(config_t * config)
{
    char buf[128];
    memset(buf, 0, 128);
    if (!gets(buf)) return -1;

    if (0 == strcmp(buf, "start"))
    {
        config->mask_state |= 0x000001;
    }
    else if (0 == strcmp(buf, "stop"))
    {
        config->mask_state ^= 0x000001;
    }
    else if (0 == strcmp(buf, "show stat"))
    {
        printf("counter = %lld\n", config->net.byte_counter);
    }
    return 0;
}