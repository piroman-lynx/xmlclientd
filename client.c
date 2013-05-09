#include "process.h"
#include "debug.h"
#include <stdlib.h>

void client_thread()
{
    logger("Worker process started: cleint", DEBUG_INFO);\
    exit(0);
}

void client_start_process(int argc, char* argv[])
{
    int result = process_make_new(&client_thread);
    if (result < 0){
	debug("can't fork client\n");
    }
}