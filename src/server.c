#include "debug.h"
#include "const.h"
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int server_epoll_create()
{
    int efd = epoll_create(10000);
    if (efd == -1)
    {
        logger("epoll_create1 failed",DEBUG_ERROR);
        return -1;
    }
    return efd;
}

void set_epoll_on_shared_socket(int efd, int listener)
{
    debug("Client is run");

    char buf[RECV_BUF*2];
    struct epoll_event *events;
    events = calloc (MAXEVENTS, sizeof (struct epoll_event));
    struct sockaddr_in their_addr;

    while (PERMANENT_CYCLE)
    {
      int n, i;
      n = epoll_wait (efd, events, MAXEVENTS, -1);
      for (i = 0; i < n; i++){
	    if(events[i].data.fd == listener){
		debug("connecting!");
		int client = accept(listener, (struct sockaddr*)&their_addr,);
		fcntl(sockfd, F_SETFL, fcntl(client, F_GETFD, 0)|O_NONBLOCK);
		//ev находится в родительском процессе и все ломает
		ev.data.fd=client;
		epoll_ctl(efd, EPOLL_CTL_ADD, client, &ev);
		debug("controled");
	    }else{
		server_handle_message(events[i].data.fd);
	    }
      }
    }
}

void server_handle_message(int client)
{
    char buf[BUF_SIZE];
    bzero(buf, BUF_SIZE);
    int len = recv(client, buf, BUF_SIZE, 0);
    if(len == 0){

    }else if (strcmp(buf,"")==0){
	debug("end of write");
        close(client);
    }else{
	debug_s(":: ",buf);
    }
}

void process_connection(int csocket)
{
    
}
