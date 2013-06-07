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
    conn->efd = client_epoll_create();
    
    buff[conn->commands_count] = malloc(sizeof(char) * READ_BUFF_SIZE);
    memset(buff[conn->commands_count], 0, READ_BUFF_SIZE);

    char *n_str;
    while ((n = read(STDIN_FILENO, buff[conn->commands_count], READ_BUFF_SIZE-1)) > 0){
	if (strlen(buff[conn->commands_count])==1){
	    break;
	}
	debug("readed!");
	n_str = malloc(32 * sizeof(char));
	sprintf(n_str, "%d", conn->commands_count);
	printf("command_count=%d\n", conn->commands_count);
	printf("command_readed=%s\n", buff[conn->commands_count]);
	printf("n_str='%s'\n", n_str);
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
	int r = openproto_run_command(buff[i], &conn /*console_efd, socket_send_hash, socket_recaive_hash, commands_hash, command_count, i, sockfd*/);
	//todo: tmp commented
	//free(buff[i]);
	int new_size = g_hash_table_size(conn->send_hash);
	if (new_size > old_size){
	    //connection open!
	    debug("connection opened");
	    //conn->sockfd = r;
	    if (!openproto_detect_write(&conn))
	    {
		break;
	    }
	}
    }
    client_start_epoll(&conn);

}