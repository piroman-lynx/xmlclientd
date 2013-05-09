#include "process.h"
#include "debug.h"
#include "server.h"

#include <stdlib.h>

void client_thread()
{
    logger("Worker process started: cleint", DEBUG_INFO);
    //todo: pass socket here
    set_epoll_on_shared_socket(0);
    exit(0);
}

void client_start_process(int argc, char* argv[])
{
    int result = process_make_new(&client_thread);
    if (result < 0){
	debug("can't fork client\n");
    }
}