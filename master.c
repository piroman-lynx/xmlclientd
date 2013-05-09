#include "master.h"
#include "process.h"
#include "debug.h"
#include "client.h"

#include "unistd.h"
#include <stdlib.h>

void master_thread()
{
    logger("Master process started: master", DEBUG_INFO);
    exit(0);
}

void master_start_process(int argc, char* argv[])
{
    int result = process_make_new(&master_thread);
    if (result < 0){
	debug("can't fork master\n");
    }
}

