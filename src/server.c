#include "debug.h"
#include "const.h"
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int server_epoll_create()
{
    int efd = epoll_create1 (0);
    if (efd == -1)
    {
        logger("epoll_create1 failed",DEBUG_ERROR);
        return -1;
    }
    return efd;
}

void set_epoll_on_shared_socket(int socket)
{
    debug("Client is run");

    char buf[RECV_BUF*2];
    struct epoll_event *events;
    events = calloc (MAXEVENTS, sizeof (struct epoll_event));

    while (PERMANENT_CYCLE)
    {
      int n, i;
      n = epoll_wait (socket, events, MAXEVENTS, -1);
      for (i = 0; i < n; i++){
            //115 - operation in progress
            if ((errno != 115) && ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))) {
                perror("Initializator: Epoll");
                logger("Initializator: epoll error!", DEBUG_ERROR);
                close (events[i].data.fd);
                continue;
            }else{
              int done = 0;
              while (1){
                  ssize_t count;
                  char buf[RECV_BUF];
                  memset(buf, 0, RECV_BUF);
                  count = read (events[i].data.fd, buf, sizeof buf);
                  if (count == -1){
                      if (errno != EAGAIN){
                          perror ("Initializator: read");
                          done = 1;
                      }
                      break;
                  } else if (count == 0) {
                      done = 1;
                      debug("Initializator: done");
                      break;
                  } else {
                      debug_s("Readed: ", buf);
                  }
              }
              if (done){
                  printf ("Initializator: Closed connection on descriptor %d\n", events[i].data.fd);
                  close (events[i].data.fd);
              }
          }
      }
    }
}

void process_connection(int csocket)
{
    
}
