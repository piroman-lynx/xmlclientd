#include <glib.h>

void client_start_process(int argc, int* argv);
void client_start_epoll(int efd, GHashTable* socket_send_hash, GHashTable* socket_recaive_hash, GHashTable* commands_hash, int command_count, int i);