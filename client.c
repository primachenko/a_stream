#include "console.h"

int main(int argc, char ** argv)
{
    printf("Client started\n");

    console_init();
    console_loop();
    console_deinit();;

    return 0;
}