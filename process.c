#include "unistd.h"
#include "const.h"
#include "debug.h"

#include <stdlib.h>
#include <sys/types.h>

int process_make_new(void (*callback)(int argc, char* argv[]), int argc, char* argv[])
{
    pid_t pid = -1;
    pid = fork();
    if (pid == 0){
	do {
	    callback(argc, argv);
	    debug("Next cycle!\n");
	} while (PERMANENT_CYCLE);
	debug("Exiting process!\n");
	exit(0);
    }else if (pid > 0){
	debug("Started process!\n");
	return 0;
    }
    return -1;
}