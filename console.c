#include "console.h"
#include "sound.h"
#include "net.h"

struct pollfd console;
static console_status_t status;
static connection_status_t connection;
static streamcast_status_t streamcast;
static sound_t * sound;
static net_t * net;
pthread_t tid;

static int is_uint8(char * val)
{
    int oct = atoi(val);
    if (oct < 0 || oct > 255) return 0;
    return 1;
}

static int is_addr(char * host)
{
    char * delim = NULL;
    delim = strtok(host, ".");
    if (NULL == delim || !is_uint8(delim)) return 0;

    delim = strtok(NULL, ".");
    if (NULL == delim || !is_uint8(delim)) return 0;

    delim = strtok(NULL, ".");
    if (NULL == delim || !is_uint8(delim)) return 0;

    delim = strtok(NULL, ".");
    if (NULL == delim || !is_uint8(delim)) return 0;

    delim = strtok(NULL, ".");
    if (NULL != delim) return 0;
    return 1;
}

static void connect_cmd(char * cmd)
{
    strtok(cmd, " ");
    char * host = strtok(NULL, " ");
    if (NULL == host)
    {
        printf("Use /connect <host>\n");
        return;
    }
    char * host_verify = strdup(host); 
    if (!is_addr(host_verify))
    {
        printf("Invalid address\n");
        free(host_verify);
        return;
    }
    free(host_verify);

    int rc = net_initialize(&net, host, PORT);
    if (0 != rc)
    {
        printf("Connect failed\n");
        return;
    }
    connection = CONNECTION_EST;
    printf("Connection to %s:%d estabilished\n", host, PORT);
}

static void disconnect_cmd(char * cmd)
{
    if (connection == CONNECTION_NO)
    {
        printf("Connection not estabilished\n");
        return;
    }
    connection = CONNECTION_NO;
    streamcast = STREAMCAST_OFF;
    int rc = net_finish(net);
    if (0 != rc)
        printf("Deinitialization network failed\n");
}

static void exit_cmd(char * cmd)
{
    if (status == CONSOLE_STOPPING) return;

    status = CONSOLE_STOPPING;
    pthread_join(tid, NULL);
    if (connection == CONNECTION_NO) return;
    int rc = net_finish(net);
    if (0 != rc)
        printf("Deinitialization network failed\n");
}

static void version_cmd(char * cmd)
{
    printf("Version: %s\n", VERSION);
}

static void streamcast_on_cmd(char * cmd)
{
    if (connection == CONNECTION_NO)
    {
        printf("Connection not estabilished\n");
        return;
    }

    if (streamcast == STREAMCAST_ON)
        printf("Streamcast already enabled\n");
    else
    {
        streamcast = STREAMCAST_ON;
        printf("Streamcast enabled\n");
    }
}

static void streamcast_off_cmd(char * cmd)
{
    if (streamcast == STREAMCAST_OFF)
        printf("Streamcast already disabled\n");
    else
    {
        streamcast = STREAMCAST_OFF;
        printf("Streamcast disabled\n");
    }
}

static void status_cmd(char * cmd)
{
    printf("App:\t\t%s\n", (status == CONSOLE_RUNNING) ? "RUNNING" : "STOPPED");
    printf("Net:\t\t%s\n", (connection == CONNECTION_EST) ? "ESTABILISHED" : "NOT ESTABILISHED");
    if (connection == CONNECTION_EST) printf("To:\t\t%s\n", net->socket);
    printf("Streamcast:\t%s\n", (streamcast == STREAMCAST_ON) ? "ENABLED" : "DISABLED");
}

static void unknow_cmd(char * cmd)
{
    printf("Unknow command: \"%s\"\nUse /help\n", &cmd[1]);
}

static void help_cmd(char * cmd)
{
    printf("Avialable commands:\n\tconnect <host>\n\tdisconnect\n\tstart\n\tstop\n\tstatus\n\tversion\n\texit\n");
}

static void console_cmd_handle(char * cmd)
{
    if (NULL == cmd) return;

    if(strstr(cmd, "disconnect"))
        disconnect_cmd(cmd);
    else if(strstr(cmd, "connect"))
        connect_cmd(cmd);
    else if(strstr(cmd, "ver"))
        version_cmd(cmd);
    else if(strstr(cmd, "status"))
        status_cmd(cmd);
    else if(strstr(cmd, "start"))
        streamcast_on_cmd(cmd);
    else if(strstr(cmd, "stop"))
        streamcast_off_cmd(cmd);
    else if(strstr(cmd, "help"))
        help_cmd(cmd);
    else if(strstr(cmd, "exit"))
        exit_cmd(cmd);
    else
        unknow_cmd(cmd);
}

static int is_cmd(char * cmd)
{
    if (cmd[0] == '/') return 1;
    return 0;
}

static void console_msg_handle(char * msg)
{
    /* outgoing message handler */
    printf("Connection for text message not estabilished\nMsg: %s\n", msg);
    return;
}

void console_loop(void * cookie)
{
    int i;
    int rc = 0;
    char buf[INPUT_MAX_LEN];

    while (CONSOLE_RUNNING == status)
    {
        rc = poll(&console, 1, POLL_TIMEOUT);
        if (rc == 0) continue;
        if (console.revents & POLLIN)
        {
            memset(buf, 0, sizeof(buf));
            for (i = 0; i < sizeof(buf) && (10 != (buf[i] = getchar()));i++);
            buf[i] = '\0';
            if (is_cmd(buf))
                console_cmd_handle(buf);
            else
                console_msg_handle(buf);
        }
        if (rc < 0) return;
    }
    return;
}

void * streamcast_loop(void * cookie)
{
    int rc = create_capture_device(&sound);
    if (0 != rc)
    {
        printf("Error creating capture device\n");
        return NULL;
    }

    message_t * message = (message_t *) malloc(sizeof(header_t) + MAX_PAYLOAD_LEN);

    while (status == CONSOLE_RUNNING)
    {
        if (streamcast == STREAMCAST_OFF)
        {
            sleep(1);
            continue;
        }
        memset(message, 0, sizeof(*message));
        message->header.length = MAX_PAYLOAD_LEN;
        rc = read_data(sound, (void *) message->payload, message->header.length);
        if (0 != rc)
        {
            printf("Error recording audio\n");
            free(message);
            status = CONSOLE_STOPPING;
            return NULL;
        }

        rc = net_send_message(net, message);
        if (0 != rc)
        {
            printf("Error sending message\n");
            free(message);
            status = CONSOLE_STOPPING;
            return NULL;
        }
    }

    free(message);

    rc = destroy_capture_sound_device(sound);
    if (0 != rc)
    {
        printf("Error destruction capture device\n");
        return NULL;
    }

    return NULL;
}

void console_init()
{
    memset(&console, 0, sizeof(console));
    console.fd = 0;
    console.events = POLLIN;

    status = CONSOLE_RUNNING;
    connection = CONNECTION_NO;
    streamcast = STREAMCAST_OFF;

    pthread_create(&tid, NULL, streamcast_loop, NULL);
}

void console_deinit()
{
    exit_cmd(NULL);
}