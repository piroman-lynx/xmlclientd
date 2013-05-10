#include <sys/epoll.h>

int create_and_bind (char *port);
int make_socket_non_blocking (int sfd);
void epool_create_real(int sfd, struct epoll_event **events, int* efd);

