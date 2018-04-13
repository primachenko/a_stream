#ifndef HANDLERS_H
#define HANDLERS_H

#include "config.h"
#include "net.h"
#include "sound.h"
#include "codec.h"

#define POLL_TIMEOUT 1

int fds_init(net_t * net);
void * fds_handle(void * data);
void * tx_pthread(void * data);
int user_input_handle(config_t * config);

#endif /* HANDLERS_H */