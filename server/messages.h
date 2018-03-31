#define MAX_PAYLOAD_LEN 1024
#include <stdint.h>
enum message_flags
{
	message_flag_min = 0,
	message_flag_new,
	message_flag_more,
	message_flag_end,
	message_flag_last = message_flag_end,
	message_flag_max = 255
};

typedef struct __attribute__((packed))
{
	uint32_t length;
	uint8_t flags;
} header_t;

typedef struct __attribute__((packed))
{
	header_t header;
	uint8_t payload[0];
} message_t;