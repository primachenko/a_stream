#ifndef HANDLERS_H
#define HANDLERS_H

#include "config.h"
#include "net.h"
#include "sound.h"

#define POLL_TIMEOUT 1

int fds_init(net_t * net);
void * fds_handle(void * data);

#endif /* HANDLERS_H */