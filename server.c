#include "debug.h"
#include "const.h"

void set_epoll_on_shared_socket(int socket)
{
    while (PERMANENT_CYCLE)
    {
	debug("Cleint is run!");
	sleep(25);
    }
}

void process_connection(int csocket)
{
    
}