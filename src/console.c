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
    int command_count = 0;
    buff[command_count] = malloc(sizeof(char) * READ_BUFF_SIZE);
    int sockfd = 0;
    int console_efd = client_epoll_create();


    GHashTable* socket_send_hash = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable* socket_recaive_hash = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable* commands_hash = g_hash_table_new(g_str_hash, g_str_equal);

    memset(buff[command_count], 0, READ_BUFF_SIZE);

    char n_str[]="\0\0\0\0\0\0\0\0\0";
    while ((n = read(STDIN_FILENO, buff[command_count], READ_BUFF_SIZE-1)) > 0){
	if (strlen(buff[command_count])==1){
	    break;
	}
	debug("readed!");
	sprintf(n_str, "%d", command_count);
	g_hash_table_insert(commands_hash, n_str, buff);
	command_count++;
	buff[command_count] = malloc(sizeof(char) * READ_BUFF_SIZE);
	memset(buff[command_count], 0, READ_BUFF_SIZE);
    }

    debug("Run Command Batch!");

    for (i=0; i<command_count; i++){
	debug("in cycle");
	int old_size = g_hash_table_size(socket_send_hash);
//	char* str_i = malloc(sizeof(char)*128);
//	debug("malloc ok");
//	sprintf(str_i, "%d", i);
//	debug("sprintf ok");
//	debug(str_i);
	//buff = g_hash_table_lookup(commands_hash, str_i);
	if (!buff[i]){
	    debug("Command Batch finished");
	    debug("Exiting");
	    return;
	}
	debug("buff ok");
//	free(str_i);
//	debug("free str_i ok");
	int r = openproto_run_command(buff[i], console_efd, socket_send_hash, socket_recaive_hash, commands_hash, command_count, i, sockfd);
	debug("command run ok");
	free(buff[i]);
	debug("free buff ok");
	int new_size = g_hash_table_size(socket_send_hash);
	if (new_size > old_size){
	    //connection open!
	    debug("connection opened");
	    sockfd = r;
	    break;
	}
    }
    client_start_epoll(console_efd);

    g_hash_table_destroy(socket_send_hash);
    g_hash_table_destroy(socket_recaive_hash);
    g_hash_table_destroy(commands_hash);
}