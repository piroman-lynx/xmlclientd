#include <glib.h>

#include "const.h"

void client_start_process(int argc, int* argv);
void client_start_epoll(struct connection **conn);
int client_epoll_create();
void client_start_watcher();
void* client_watcher_entry_point();
void client_watcher_add(int sfd);

void wait_watcher_lock_and_lock();
void wait_watcher_lock_unlockk();

