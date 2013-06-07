#include <glib.h>
#include "const.h"

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
#define OPENPROTO_STR_WRITE "Write("

//do detect and run command
int openproto_run_command(char* string, struct connection **conn);
int openproto_next_read_command(struct connection **conn);
int openproto_detect_write(struct connection **conn);

int openproto_detect_command(char* string);
char openproto_parse(char* string, char** value, unsigned int* event);

int openproto_run_CONNECT(char* uri, unsigned int event, int console_efd, GHashTable* socket_send_hash, GHashTable* socket_recaive_hash);
void openproto_run_CLOSE(unsigned int event);
char* openproto_run_READ(unsigned int event, char* value, GHashTable* socket_send_hash, GHashTable* socket_recaive_hash, int sockfd);
void openproto_run_WRITE(char* value, int sockfd);
