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
    int n;
    char buff[READ_BUFF_SIZE];
    memset(buff, 0, READ_BUFF_SIZE);
    int console_efd = client_epoll_create();

    GHashTable* socket_send_hash = g_hash_table_new(g_str_hash, g_str_equal);
    GHashTable* socket_recaive_hash = g_hash_table_new(g_str_hash, g_str_equal);

    int sockfd = 0;

    while ((n = read(STDIN_FILENO, buff, READ_BUFF_SIZE-1)) > 0){
	int old_size = g_hash_table_size(socket_send_hash);
	printf("count of connections: %d\n",old_size);
	int r = openproto_run_command(buff, console_efd, socket_send_hash, socket_recaive_hash, sockfd);
	int new_size = g_hash_table_size(socket_send_hash);
	printf("count of connections: %d\n",new_size);
	if (new_size > old_size){
	    //connection open!
	    sockfd = r;
	}else if (new_size < old_size){
	    //connection closed!
	    sockfd = 0;
	}
	memset(buff, 0, READ_BUFF_SIZE);
    }

    g_hash_table_destroy(socket_send_hash);
    g_hash_table_destroy(socket_recaive_hash);
}