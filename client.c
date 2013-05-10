#include "process.h"
#include "debug.h"
#include "server.h"

#include <stdlib.h>

void client_thread(int argc, int* argv)
{
    logger("Worker process started: cleint", DEBUG_INFO);
    //todo: pass socket here
    if (argc > 0){
	set_epoll_on_shared_socket(argv[1]);
    }
    exit(0);
}

int client_epoll_create()
{
    int efd = epoll_create1 (0);
    if (efd == -1)
    {
	logger("epoll_create1 failed",DEBUG_ERROR);
	return -1;
    }
    return efd;
}

void client_start_process(int argc, int* argv)
{
    int result = process_make_new(&client_thread, argc, argv);
    if (result < 0){
	debug("can't fork client\n");
    }
}