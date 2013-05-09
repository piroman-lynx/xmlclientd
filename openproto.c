#include "openproto.h"
#include "debug.h"
#include "strpos.h"

#include <stdlib.h>
#include <string.h>

void openproto_run_command(char* string)
{
    int command = openproto_detect_command(string);
    if (command < 0){
	logger("Bad command", DEBUG_WARN);
	return;
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

int openproto_detect_command(char* string)
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
    trim(&string);
    int startEvent = strpos("(", string);
    int endEvent = strpos(")", string);
    int length = endEvent-startEvent;
    if (length < 1){
	logger("Bad Comamnd Format", DEBUG_WARN);
	return 0;
    }
    char* eventString = malloc(sizeof(char) * (length+1));
    int i,j=0;
    for (i=startEvent+1; i<endEvent; i++){
	eventString[j] = string[i];
	j++;
    }
    eventString[j]=0;
    (*event) = atoi(eventString);
    free(eventString);
    //test:
    printf("strlen: %d, endEvent+2: %d\n", strlen(string), (endEvent+2));
    if (strlen(string) < (endEvent+2)){
	logger("Bad Comamnd Format, empty value", DEBUG_WARN);
	return 0;
    }
    j=0;
    char *str = malloc(sizeof(char) * (strlen(string) + 1));
    for (i=endEvent+1; i<strlen(string); i++){
	str[j] = string[i];
	j++;
    }
    str[j]=0;
    (*value) = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy((*value), str);
    free(str);
    return 1;
}

void openproto_run_CONNECT(char* uri, unsigned int event)
{
    debug("Connect!");
    debug(uri);
    free(uri);
}

void openproto_run_CLOSE(unsigned int event)
{
    debug("Close!");
}

char* openproto_run_READ(unsigned int event)
{
    debug("Read!");
}
