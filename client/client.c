#include "config.h"
#include "net.h"
#include "sound.h"

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

    rc = open_socket(&config->net);
    if (0 != rc)
    {
        printf("open_socket failed\n");
        return -1;
    }

    rc = create_capture_device(&config->sound);
    if (0 != rc)
    {
        printf("create_capture_device failed\n");
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
        rc = read_data(&config->sound, buf, MAX_PAYLOAD_LEN);
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

    rc = destroy_capture_sound_device(&config->sound);
    if (0 != rc)
    {
        printf("destroy_sound_device failed\n");
        return -1;
    }

    rc = close_socket(&config->net);
    if (0 != rc)
    {
        printf("close_socket failed\n");
        return -1;
    }

    free(config);

    return 0;
}