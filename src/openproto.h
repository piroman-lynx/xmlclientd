#include <glib.h>

#define OPENPROTO_CONNECT 1
#define OPENPROTO_CLOSE 2
#define OPENPROTO_WAITCONNECT 3
#define OPENPROTO_WAITTIMEOUT 4
#define OPENPROTO_READ 5
#define OPENPROTO_WRITE 6
#define OPENPROTO_MATCH 7
#define OPENPROTO_EXISTS 7

#define OPENPROTO_STR_CONNECT "Connect("
#define OPENPROTO_STR_CLOSE "Close("
#define OPENPROTO_STR_READ "Read("

//do detect and run command
int openproto_run_command(char* string, int console_efd, GHashTable* socket_send_hash, GHashTable* socket_recaive_hash, GHashTable* command, int command_count, int step, int sockfd);
int openproto_next_read_command(GHashTable* socket_recaive_hash, int client_fd, GHashTable* commands_hash, int ecounter, int command_count, int efd, int sockfd);

int openproto_detect_command(char* string);
char openproto_parse(char* string, char** value, unsigned int* event);

int openproto_run_CONNECT(char* uri, unsigned int event, int console_efd, GHashTable* socket_send_hash, GHashTable* socket_recaive_hash);
void openproto_run_CLOSE(unsigned int event);
char* openproto_run_READ(unsigned int event, GHashTable* socket_send_hash, GHashTable* socket_recaive_hash, int sockfd);
