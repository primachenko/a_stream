#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pulse/simple.h>
#include <pulse/error.h>

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

#define ADDR            "127.0.0.1"
#define MAX_PAYLOAD_LEN 1024
#define PORT            32734
#define TIMEOUT         1
#define INPUT_MAX_LEN   128

typedef struct __attribute__((packed))
{
    uint32_t length;
} header_t;

typedef struct __attribute__((packed))
{
    header_t header;
    uint8_t payload[0];
} message_t;

static uint8_t stopped = 0;

int input_handle(struct pollfd * fds, uint8_t nfds);
int command_handle(char * cmd);

int main(int argc, char ** argv)
{
    int32_t rcode;
    int32_t fd;
    uint8_t nfds = 1;
    struct pollfd fds;

    struct sockaddr c_addr;
    struct sockaddr s_addr;

    printf("Client started\n");

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > fd)
    {
        printf("Error: Create socket failed\n");
        return fd;
    }

    // struct sockaddr_in * addr = (struct sockaddr_in *) &c_addr;
    // memset(addr, 0, sizeof(*addr));
    // addr->sin_family = AF_INET;
    // addr->sin_addr = inet_addr();
    // addr->sin_port = htonl(PORT);

    memset(&fds, 0, sizeof(fds));
    fds.fd = 0;
    fds.events = POLLIN;

    while (0 == stopped)
    {
        rcode = poll(&fds, nfds, TIMEOUT);
        if (rcode == 0) continue;
        else if (rcode < 0)
        {
            printf("Error: poll failed\n");
            close(fd);
            return -1;
        }
        else
        {
            rcode = input_handle(&fds, nfds);
            if (0 != rcode)
            {
                printf("Error: input handle failed\n");
                close(fd);
                return -1;
            }
        }
    }

    return 0;
}

int input_handle(struct pollfd * fds, uint8_t nfds)
{
    int i;
    int rcode;
    char buf[INPUT_MAX_LEN];
    memset(buf, 0, sizeof(buf));

    if (fds->revents & POLLIN)
    {
        for (i = 0; i < sizeof(buf) && (10 != (buf[i] = getchar()));i++);
        buf[i] = '\0';
        rcode = command_handle(buf);
        if (0 != rcode)
        {
            printf("Error: cannot handle command\n");
            return -1;
        }
    }

    return 0;
}

int command_handle(char * cmd)
{
    if(strstr(cmd, "connect"))
        printf("connect cmd\n");
    else if(strstr(cmd, "disconnect"))
        printf("disconnect cmd\n");
    else if(strstr(cmd, "exit"))
        stopped = 1;
    return 0;
}