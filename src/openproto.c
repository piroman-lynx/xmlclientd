#include "openproto.h"
#include "debug.h"
#include "strpos.h"
#include "network.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int openproto_run_command(char* string, struct connection **conn /*int console_efd, GHashTable *send, GHashTable *recaive, GHashTable *commands, int command_count, int step, int sockfd*/)
{
    int command = openproto_detect_command(string);
    if (command < 0){
	logger("Bad command", DEBUG_WARN);
	return;
    }
    unsigned int event;
    char* value;
    char* returned;
    int sock;

    char success = openproto_parse(string, &value, &event);

    switch (command){
	case OPENPROTO_CONNECT:
	    sock = openproto_run_CONNECT(value, event, (*conn)->efd, (*conn)->send_hash, (*conn)->recaive_hash);
	    printf("openproto_run_command/connect sockfd_origin: %d\n",sock);
	    (*conn)->sockfd = sock;
	    printf("openproto_run_command/connect sockfd: %d\n",(*conn)->sockfd);
	    return sock;
	    break;
	case OPENPROTO_CLOSE:
	    openproto_run_CLOSE(event);
	    return 0;
	    break;
	case OPENPROTO_READ:
	    printf("sockfd: %d\n",(*conn)->sockfd);
	    openproto_run_READ(event, value, (*conn)->send_hash, (*conn)->recaive_hash, (*conn)->sockfd);
	    return 0;
	    break;
	case OPENPROTO_WRITE:
	    printf("sockfd: %d, write\n",(*conn)->sockfd);
	    openproto_run_WRITE(value, (*conn)->sockfd);
	    return 0;
	    break;
	default:
	    logger("Not Implemented!", DEBUG_ERROR);
    }
}

int openproto_detect_write(struct connection **conn)
{
    int icounter = (*conn)->now_command + 1;
    char* str_icounter = malloc(sizeof(char) * 1024);
    memset(str_icounter, 0 , 1024);
    sprintf(str_icounter, "%d", icounter);
    printf("count: %d\n", g_hash_table_size((*conn)->commands_hash));
    printf("str_icounter: '%s'\n", str_icounter);
    char* nextcommand = g_hash_table_lookup((*conn)->commands_hash, str_icounter);
    debug("Detecting!");
    printf("String: %s\n", nextcommand);
    printf("(*conn): %i\n", (*conn));
    printf("commands_hash: %i\n", (*conn)->commands_hash);
    int i = openproto_detect_command(nextcommand);
    debug("getted!");
    if (i == OPENPROTO_WRITE){
        debug("Detected write");
	return 1;
    }else{
	debug("Detected read");
	return 0;
    }
}

int openproto_next_read_command(struct connection **conn)
{
    //get data from buffer

    //check next command - Read(X) STRING or Read(X) ANY
    int icounter = (*conn)->now_command + 1;
    printf("ecounter=%d\n",icounter);
    char* str_icounter = malloc(sizeof(char) * 1024);
    memset(str_icounter, 0 , 1024);
    sprintf(str_icounter, "%d", icounter);
    debug("count commands in batch: ");
    printf("count: %d\n", g_hash_table_size((*conn)->commands_hash));
    char* nextcommand = g_hash_table_lookup((*conn)->commands_hash, str_icounter);
    if (!nextcommand){
	debug("Command batch is empty");
    }else{
	debug("Next Command:");
	debug(nextcommand);
    }
    (*conn)->now_command = icounter;
    printf("run_next_read_command sockfd: %d\n",(*conn)->sockfd);
    icounter = openproto_run_command(nextcommand, conn);
    return icounter;
}

int openproto_detect_command(char* string)
{
    if (strpos(OPENPROTO_STR_CONNECT, string) == 0){
	return OPENPROTO_CONNECT;
    }else
    if (strpos(OPENPROTO_STR_CLOSE, string) == 0){
	return OPENPROTO_CLOSE;
    }else
    if (strpos(OPENPROTO_STR_WRITE, string) == 0){
	return OPENPROTO_WRITE;
    }else
    if (strpos(OPENPROTO_STR_READ, string) == 0){
	return OPENPROTO_READ;
    }else{
	return -1;
    }
}

/**
* Format:
* CommandName(EventId) text
**/
char openproto_parse(char* string, char** value, unsigned int* event)
{
    trim(&string);
    int startEvent = strpos("(", string);
    int endEvent = strpos(")", string);
    int length = endEvent-startEvent;
    if (length < 1){
	logger("Bad Comamnd Format", DEBUG_WARN);
	return 0;
    }
    char* eventString = malloc(sizeof(char) * (length+1));
    int i,j=0;
    for (i=startEvent+1; i<endEvent; i++){
	eventString[j] = string[i];
	j++;
    }
    eventString[j]=0;
    (*event) = atoi(eventString);
    free(eventString);
    //test:
    if (strlen(string) < (endEvent+2)){
	logger("Bad Comamnd Format, empty value", DEBUG_WARN);
	return 0;
    }
    j=0;
    char *str = malloc(sizeof(char) * (strlen(string) + 1));
    for (i=endEvent+1; i<strlen(string); i++){
	str[j] = string[i];
	j++;
    }
    str[j]=0;
    (*value) = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy((*value), str);
    free(str);
    return 1;
}

int openproto_run_CONNECT(char* uri, unsigned int event, int console_efd, GHashTable *send, GHashTable *recaive)
{
    debug("Connect!");
    debug(uri);
    int proto, port;
    struct hostent* host;
    if (url_parse(uri, &proto, &host, &port) < 0){
	logger("Can't parse url",DEBUG_WARN);
	return -1;
    }
    free(uri);
    int sockfd, n;
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
	logger("Can't create client socket",DEBUG_ERROR);
	return -2;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)host->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          host->h_length);
    serv_addr.sin_port = htons(port);
    if (make_socket_non_blocking(sockfd) != 0){
	logger("can't make socket non-blocking",DEBUG_ERROR);
	return -3;
    }
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
	if (errno != 115){
		logger("can't connect client socket",DEBUG_ERROR);
		return -4;
	}else{
		debug("Connect: operation in progress");
	}
    }
    struct epoll_event *revent = malloc(sizeof(struct epoll_event));
    revent->data.fd = sockfd;
    revent->events = EPOLLIN | EPOLLET;
    if (epoll_ctl(console_efd, EPOLL_CTL_ADD, sockfd, revent) == -1){
	logger("can't epoll_ctl", DEBUG_ERROR);
	return -5;
    }
    printf("socket: %d\n", sockfd);

    char* sock_str = malloc(128 * sizeof(char));
    sprintf(sock_str, "%d", sockfd);
    printf("sock_str: %s\n", sock_str);
    g_hash_table_insert(send, sock_str, "");
    g_hash_table_insert(recaive, sock_str, "");
    //free(sock_str);
    return sockfd;
}

void openproto_run_CLOSE(unsigned int event)
{
    debug("Close!");
}

char* openproto_run_READ(unsigned int event, char* value, GHashTable *send, GHashTable *recaive, int sockfd)
{
    char* sock_str = malloc(128 * sizeof(char));
    sprintf(sock_str, "%d", sockfd);

    char* recaived = g_hash_table_lookup(recaive, sock_str);
    debug("Read!");
    printf("sock_str: %s\n", sock_str);
    printf("Bytes: %d, value %s\n", strlen(recaived), recaived);
    printf("Command Type: %s\n",value);
    if (strpos("STRING",value) == 0){
	
	//split first string, serve strings (run next command: READ or MATCH or CLOSE), increment command counters
    }else{
	//increment command counters, run next command: MATCH or CLOSE
    }
}

void openproto_run_WRITE(char* value, int sockfd)
{
    debug("Write!");
    char* sock_str = malloc(128 * sizeof(char));
    sprintf(sock_str, "%d", sockfd);
    printf("Bytes: %d, value %s\n", strlen(value), value);
    send(sockfd, value, strlen(value)+1, 0);
}
