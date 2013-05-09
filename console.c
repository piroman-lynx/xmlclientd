#include "console.h"
#include "const.h"
#include "openproto.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void console_start(int argc, char* argv[])
{
    int n;
    char buff[READ_BUFF_SIZE];
    memset(buff, 0, READ_BUFF_SIZE);
    while ((n = read(STDIN_FILENO, buff, READ_BUFF_SIZE-1)) > 0){
	openproto_run_command(buff);
	memset(buff, 0, READ_BUFF_SIZE);
    }
}