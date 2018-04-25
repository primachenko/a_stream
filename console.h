#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <pthread.h>

#define VERSION        "0.0.0.3"
#define POLL_TIMEOUT    1000
#define INPUT_MAX_LEN   1024

typedef enum
{
    CONSOLE_RUNNING,
    CONSOLE_STOPPING
} console_status_t;

void console_init();
void console_loop();
void console_deinit();

#endif /* CONSOLE_H */