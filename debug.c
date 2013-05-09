#include "debug.h"
#include <stdio.h>

char levels[][6] = { "FATAL", "ERROR", "WARN ", "INFO ", "     ", "     ", "DEBUG" };

void logger(char* message, int level)
{
    printf("[%d] [%s] %s", 0, levels[level], message);
}

void debug(char* message)
{
    logger(message, DEBUG_DEBUG);
}