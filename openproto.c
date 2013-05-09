#include "openproto.h"
#include "debug.h"

#include <stdlib.h>
#include <string.h>

void openproto_run_command(char* string)
{
    unsigned int command = openproto_detect_command(string);
    if (command < 0){
	logger("Bad command", DEBUG_WARN);
    }
    unsigned int event;
    char* value;


    char* returned;

    char success = openproto_parse(string, &value, &event);

    switch (command){
	case OPENPROTO_CONNECT:
	    openproto_run_CONNECT(value, event);
	    break;
	case OPENPROTO_CLOSE:
	    openproto_run_CLOSE(event);
	    break;
	case OPENPROTO_READ:

	    returned = openproto_run_READ(event);
	    break;
	default:
	    logger("Not Implemented!", DEBUG_ERROR);
    }
}

unsigned int openproto_detect_command(char* string)
{
    if (strpos(OPENPROTO_STR_CONNECT, string) == 0){
	return OPENPROTO_CONNECT;
    }else
    if (strpos(OPENPROTO_STR_CLOSE, string) == 0){
	return OPENPROTO_CLOSE;
    }else
    if (strpos(OPENPROTO_STR_READ, string) == 0){
	return OPENPROTO_READ;
    }else{
	return -1;
    }
}

/**
* Format:
* CommandName(EventId) text
**/
char openproto_parse(char* string, char** value, unsigned int* event)
{
    int startEvent = strpos("(", string);
    int endEvent = strpos(")", string);
    if (endEvent-startEvent < 1){
	logger("Bad Comamnd Format", DEBUG_WARN);
	return 0;
    }
    char* eventString = malloc(sizeof(char) * endEvent-startEvent);
    (*event) = atoi(eventString);
    free(eventString);
    return 1;
}
