#include "master.h"
#include "process.h"
#include "debug.h"
#include "client.h"
#include "const.h"
#include "network.h"

#include "unistd.h"
#include <stdlib.h>
#include <sys/socket.h>

void master_forks(int count, int socket)
{
    int i;
    for (i=0; i<count; i++){
	int* argv = malloc(sizeof(int) * 1);
	argv[0] = socket;
	client_start_process(1, argv);
    }
}

void master_permanenet_wait()
{
    while (PERMANENT_CYCLE){
	sleep(60);
    }
}

/*
* pass argv here
*/
void master_thread(int argc, int* argv)
{
    logger("Master process started: master", DEBUG_INFO);
    int socket = create_and_bind("1234");
    int s = make_socket_non_blocking (socket);
    listen (socket, SOMAXCONN);
    if (socket){
	master_forks(3, socket);
    }
    master_permanenet_wait();
}

void master_start_process(int argc, int* argv)
{
    int result = process_make_new(&master_thread, argc, argv);
    if (result < 0){
	debug("can't fork master\n");
    }
}

