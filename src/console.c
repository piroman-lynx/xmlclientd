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
	n_str = malloc(32 * sizeof(char));
	sprintf(n_str, "%d", conn->commands_count);
	g_hash_table_insert(conn->commands_hash, n_str, buff[conn->commands_count]);
	conn->commands_count++;
	buff[conn->commands_count] = malloc(sizeof(char) * READ_BUFF_SIZE);
	memset(buff[conn->commands_count], 0, READ_BUFF_SIZE);
    }

    debug("Run Command Batch!");
    int opened=0;

    debug("Run Command (hard)");
    int old_size = g_hash_table_size(conn->send_hash);
    int r = openproto_run_command(buff[0], &conn);
    if (r>0){
	for (i=0; i<conn->commands_count; i++){
	    debug("in cycle");
	    if (!buff[i]){
		debug("Command Batch finished");
		debug("Exiting");
		return;
	    }
	    int new_size = g_hash_table_size(conn->send_hash);
	    if (new_size > old_size || opened==1){
		//connection open!
		debug("connection opened");
		opened=1;
		if (openproto_detect_write(&conn) == 0){
		    break;
		}else{
		    debug("Run Command (soft)");
		    //printf("command for run (soft): %s\n", buff[i]);
		    int r = openproto_run_command(buff[i], &conn);
		    if (r<0){
			break;
		    }
		}
	    }else{
		sleep(1);
		i--;
	    }
	}
    }
    debug("Client start epoll");
    client_start_epoll(&conn);
}