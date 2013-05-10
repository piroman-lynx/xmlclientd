#include "process.h"
#include "debug.h"
#include "server.h"

#include <stdlib.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>

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

void client_start_epoll(int efd)
{
    char buf[1024];
    struct epoll_event *events;
    events = calloc (MAXEVENTS, sizeof (struct epoll_event));

    while (1){
      int n, i;
      n = epoll_wait (efd, events, MAXEVENTS, -1);
      for (i = 0; i < n; i++){
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) {
                fprintf (stderr, "epoll error\n");
	        close (events[i].data.fd);
    		continue;
    	    }else{
              int done = 0;
              while (1){
                  ssize_t count;
                  char buf[512];
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
                  }
              }if (done){
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