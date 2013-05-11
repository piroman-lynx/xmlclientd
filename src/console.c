#include "console.h"
#include "const.h"
#include "openproto.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

void console_start(int argc, char* argv[])
{
    int n,i;
    char **buff = malloc(sizeof(char*) * READ_BUFF_SIZE);
    
    struct connection *conn = init_connection();
    debug("inited");
    conn->efd = client_epoll_create();
    debug("epoll");
    
    printf("commands_count: %d\n", conn->commands_count);
    buff[conn->commands_count] = malloc(sizeof(char) * READ_BUFF_SIZE);
    debug("malloc");
    memset(buff[conn->commands_count], 0, READ_BUFF_SIZE);
    debug("memset");

    char n_str[]="\0\0\0\0\0\0\0\0\0";
    while ((n = read(STDIN_FILENO, buff[conn->commands_count], READ_BUFF_SIZE-1)) > 0){
	if (strlen(buff[conn->commands_count])==1){
	    break;
	}
	debug("readed!");
	sprintf(n_str, "%d", conn->commands_count);
	printf("command_count=%d\n", conn->commands_count);
	g_hash_table_insert(conn->commands_hash, n_str, buff[conn->commands_count]);
	conn->commands_count++;
	buff[conn->commands_count] = malloc(sizeof(char) * READ_BUFF_SIZE);
	memset(buff[conn->commands_count], 0, READ_BUFF_SIZE);
    }

    debug("Run Command Batch!");
    for (i=0; i<conn->commands_count; i++){
	debug("in cycle");
	int old_size = g_hash_table_size(conn->send_hash);
	if (!buff[i]){
	    debug("Command Batch finished");
	    debug("Exiting");
	    return;
	}
	debug("Run Command");
	int r = openproto_run_command(buff[i], conn /*console_efd, socket_send_hash, socket_recaive_hash, commands_hash, command_count, i, sockfd*/);
	free(buff[i]);
	int new_size = g_hash_table_size(conn->send_hash);
	if (new_size > old_size){
	    //connection open!
	    debug("connection opened");
	    conn->sockfd = r;
	    break;
	}
    }
    client_start_epoll(conn->efd, conn->send_hash, conn->recaive_hash, conn->commands_hash, conn->commands_count, i);

}