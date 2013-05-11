#include "process.h"
#include "debug.h"
#include "server.h"

#include <stdlib.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <glib.h>
#include <string.h>

#define MAXEVENTS 1024

void client_thread(int argc, int* argv)
{
    logger("Worker process started: cleint", DEBUG_INFO);
    //todo: pass socket here
    if (argc > 0){
	set_epoll_on_shared_socket(argv[1]);
    }
    exit(0);
}

void rpush_to_buff(GHashTable* socket_recaive_hash, int socket, GHashTable* commands_hash, char* buff)
{
    debug("push_to_buff!");

    char* str_socket = malloc(sizeof(char) * 128);
    sprintf(str_socket, "%d", socket);

    char* str = g_hash_table_lookup(socket_recaive_hash, str_socket);

    if (!str){
	str = malloc(sizeof(char) * 1024);
	memset(str, 0, (1024*sizeof(char)));
    }
    
    char* tmpstr = malloc(sizeof(char) * (strlen(str)+1));
    memcpy(tmpstr, str, sizeof(char) * (strlen(str)+1));
    //free(str); //TODO: Segmentation fault here
    debug("free");
    str = malloc(sizeof(char) * (strlen(str)+strlen(buff)+1));
    memcpy(str, tmpstr, sizeof(char) * (strlen(str)+1));

    int oldlen = strlen(str);
    int i,j=0;
    for (i=oldlen; i<oldlen+strlen(buff); i++){
	str[i] = buff[j];
	j++;
    }
    str[i] = '\0';
    debug("free str_socket");
    free(str_socket);
    return;
}


void client_start_epoll(int efd, GHashTable* socket_send_hash, GHashTable* socket_recaive_hash, GHashTable* commands_hash, int command_count, int ecounter)
{
    char buf[1024];
    struct epoll_event *events;
    events = calloc (MAXEVENTS, sizeof (struct epoll_event));

    int icounter=ecounter;

    printf("efd=%d\n", efd);

    while (1){
      int n, i;
      n = epoll_wait (efd, events, MAXEVENTS, -1);
      for (i = 0; i < n; i++){
	    //115 - operation in progress
            if ((errno != 115) && ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))) {
		perror("Epoll");
		printf("epoll errno=%d\n",errno);
		logger("epoll error!", DEBUG_ERROR);
		close (events[i].data.fd);
		continue;
	    }else{
              int done = 0;
              while (1){
                  ssize_t count;
                  char buf[512];
		  memset(buf, 0, 512);
                  count = read (events[i].data.fd, buf, sizeof buf);
                  if (count == -1){
                      if (errno != EAGAIN){
                          perror ("read");
                          done = 1;
                      }
                      break;
                  } else if (count == 0) {
                      done = 1;
                      break;
                  } else {
		    debug("Readed!:");
		    debug(buf);
		    rpush_to_buff(socket_recaive_hash, events[i].data.fd, commands_hash, buf);
		  }
              }
	      if (done){
                  printf ("Closed connection on descriptor %d\n", events[i].data.fd);
                  close (events[i].data.fd);
              }
          }
      }
   }
}


int client_epoll_create()
{
    int efd = epoll_create1 (0);
    if (efd == -1)
    {
	logger("epoll_create1 failed",DEBUG_ERROR);
	return -1;
    }
    return efd;
}

void client_start_process(int argc, int* argv)
{
    int result = process_make_new(&client_thread, argc, argv);
    if (result < 0){
	debug("can't fork client\n");
    }
}