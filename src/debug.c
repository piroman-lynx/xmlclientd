#include "debug.h"
#include "strpos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char levels[][6] = { "FATAL", "ERROR", "WARN ", "INFO ", "     ", "     ", "DEBUG" };

void logger(char* message, int level)
{
    char *msg = rtrim(&message);
    printf("[%d] [%s] %s\n", 0, levels[level], msg);
    free(msg);
}

void info(char* message)
{
    logger(message, DEBUG_INFO);
}

void debug(char* message)
{
    logger(message, DEBUG_DEBUG);
}

void debug_s(char *message, char *string)
{
    char *full = malloc(sizeof(char)*(strlen(message)+strlen(string)+1));
    memcpy(full, message, strlen(message));
    memcpy(full+strlen(message), string, strlen(string)+1);
    debug(full);
    free(full);
}