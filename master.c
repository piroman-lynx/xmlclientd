#include "master.h"
#include "process.h"
#include "debug.h"
#include "client.h"
#include "const.h"

#include "unistd.h"
#include <stdlib.h>

void master_forks(int count)
{
    int i;
    for (i=0; i<count; i++){
	client_start_process(0, 0);
    }
}

/*
* pass argv here
*/
int master_bind_public_address()
{
    return 1;
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
void master_thread()
{
    logger("Master process started: master", DEBUG_INFO);
    int result = master_bind_public_address();
    if (result){
	master_forks(3);
    }
    master_permanenet_wait();
}

void master_start_process(int argc, char* argv[])
{
    int result = process_make_new(&master_thread);
    if (result < 0){
	debug("can't fork master\n");
    }
}

