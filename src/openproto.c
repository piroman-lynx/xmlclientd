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

int openproto_run_command(char* string, int console_efd, GHashTable *send, GHashTable *recaive, GHashTable *commands, int command_count, int step, int sockfd)
{
    int command = openproto_detect_command(string);
    if (command < 0){
	logger("Bad command", DEBUG_WARN);
	return;
    }
    unsigned int event;
    char* value;

    char* returned;

    char success = openproto_parse(string, &value, &event);

    switch (command){
	case OPENPROTO_CONNECT:
	    return openproto_run_CONNECT(value, event, console_efd, send, recaive);
	    break;
	case OPENPROTO_CLOSE:
	    openproto_run_CLOSE(event);
	    return 0;
	    break;
	case OPENPROTO_READ:
	    returned = openproto_run_READ(event, send, recaive, sockfd);
	    return 0;
	    break;
	default:
	    logger("Not Implemented!", DEBUG_ERROR);
    }
}

int openproto_detect_command(char* string)
{
    if (strpos(OPENPROTO_STR_CONNECT, string) == 0){
	return OPENPROTO_CONNECT;
    }else
    if (strpos(OPENPROTO_STR_CLOSE, string) == 0){
	return OPENPROTO_CLOSE;
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
	return;
    }
    free(uri);
    int sockfd, n;
    struct sockaddr_in serv_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
	logger("Can't create client socket",DEBUG_ERROR);
	return;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)host->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          host->h_length);
    serv_addr.sin_port = htons(port);
    if (make_socket_non_blocking(sockfd) != 0){
	logger("can't make socket non-blocking",DEBUG_ERROR);
	return;
    }
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
	if (errno != 115){
		logger("can't connect client socket",DEBUG_ERROR);
		return;
	}else{
		debug("Connect: operation in progress");
	}
    }
    struct epoll_event *revent = malloc(sizeof(struct epoll_event));
    revent->data.fd = sockfd;
    revent->events = EPOLLIN | EPOLLET;
    if (epoll_ctl(console_efd, EPOLL_CTL_ADD, sockfd, revent) == -1){
	logger("can't epoll_ctl", DEBUG_ERROR);
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

char* openproto_run_READ(unsigned int event, GHashTable *send, GHashTable *recaive, int sockfd)
{
    char* sock_str = malloc(128 * sizeof(char));
    sprintf(sock_str, "%d", sockfd);

    char* recaived = g_hash_table_lookup(recaive, sock_str);
    debug("Read!");
    printf("Bytes: %d, value %s\n", strlen(recaived), recaived);
}

