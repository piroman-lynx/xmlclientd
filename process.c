#include "unistd.h"
#include "const.h"
#include "debug.h"

#include <stdlib.h>
#include <sys/types.h>

int process_make_new(void (*callback)(void))
{
    pid_t pid = -1;
    pid = fork();
    if (pid == 0){
	do {
	    callback();
	} while (PERMANENT_CYCLE);
	debug("Exiting process!\n");
	exit(0);
    }else if (pid > 0){
	debug("Started process!\n");
	return 0;
    }
    return -1;
}