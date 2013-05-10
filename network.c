#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <memory.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>

#include "const.h"

int* sockets; 
int sockets_counter = 0;

void alloc_sockets()
{
    sockets = malloc(sizeof(int) * 1024);
    sockets_counter = 0;
}

int create_and_bind (char *port)
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s, sfd;

  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_INET;       /* Return IPv4 and IPv6 choices */
  hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
  hints.ai_flags = AI_PASSIVE;     /* All interfaces */

  s = getaddrinfo (NULL, port, &hints, &result);
  if (s != 0)
    {
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
      return -1;
    }

  for (rp = result; rp != NULL; rp = rp->ai_next)
    {
      sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sfd == -1)
        continue;

      s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
      if (s == 0)
        {
          /* We managed to bind successfully! */
          break;
        }

      close (sfd);
    }

  if (rp == NULL)
    {
      fprintf (stderr, "Could not bind\n");
      return -1;
    }

  freeaddrinfo (result);

  return sfd;
}


int make_socket_non_blocking (int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      perror ("fcntl");
      return -1;
    }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      perror ("fcntl");
      return -1;
    }

  return 0;
}

void epool_create_real(int sfd, struct epoll_event **events, int* efd)
{
  (*efd) = epoll_create1 (0);
  struct epoll_event event;

  if ((*efd) == -1)
    {
      perror ("epoll_create");
      return;
    }

  event.data.fd = sfd;
  event.events = EPOLLIN | EPOLLET;
  int s = epoll_ctl ((*efd), EPOLL_CTL_ADD, sfd, &event);
  if (s == -1)
    {
      perror ("epoll_ctl");
      (*efd) = -1;
      return;
    }

  /* Buffer where events are returned */
  (*events) = calloc (MAXEVENTS, sizeof event);
}
