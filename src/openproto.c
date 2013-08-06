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

int openproto_run_command(char* rstring, struct connection **conn /*int console_efd, GHashTable *send, GHashTable *recaive, GHashTable *commands, int command_count, int step, int sockfd*/)
{
    int icounter = (*conn)->now_command;
    char* str_icounter = malloc(sizeof(char) * 1024);
    memset(str_icounter, 0 , 1024);
    sprintf(str_icounter, "%d", icounter);
    char* string = g_hash_table_lookup((*conn)->commands_hash, str_icounter);


    int command = openproto_detect_command(string);
    if (command < 0){
	error("Bad command");
	return;
    }
    unsigned int event;
    char* value;
    char* returned;
    int sock;
    int w;

    char success = openproto_parse(string, &value, &event);

    icounter++;

    switch (command){
	case OPENPROTO_CONNECT:
	    sock = openproto_run_CONNECT(value, event, (*conn)->efd, (*conn)->send_hash, (*conn)->recaive_hash);
	    (*conn)->sockfd = sock;
	    debug("openproto_run_command/connect");
	    (*conn)->now_command = icounter;
	    client_watcher_add(conn);
	    return sock;
	    break;
	case OPENPROTO_CLOSE:
	    debug("openproto_run_command/close");
	    openproto_run_CLOSE(event, (*conn)->sockfd);
	    (*conn)->now_command = icounter;
	    return 0;
	    break;
	case OPENPROTO_READ:
	    debug("openproto_run_command/read");
	    if (!success){
		value = malloc(sizeof(char)*2);
		strcpy(value, "\n");
	    }
	    char *readed = openproto_run_READ(event, value, (*conn)->send_hash, (*conn)->recaive_hash, (*conn)->sockfd);
	    if (readed == (char*)-1){
		return -1;
	    }
	    (*conn)->now_command = icounter;
	    return 0;
	    break;
	case OPENPROTO_WRITELN:
	    debug("openproto_run_command/writeln");
	    w = openproto_run_WRITELN(value, (*conn)->sockfd);
	    if (w>0){
		(*conn)->now_command = icounter;
	    }
	    return w;
	    break;
	case OPENPROTO_WRITE:
	    debug("openproto_run_command/write");
	    w = openproto_run_WRITE(value, (*conn)->sockfd);
	    if (w>0){
		(*conn)->now_command = icounter;
	    }
	    return w;
	    break;
	default:
	    error("Not Implemented!", DEBUG_ERROR);
    }
}

int openproto_detect_write(struct connection **conn)
{
    int icounter = (*conn)->now_command;
    char* str_icounter = malloc(sizeof(char) * 1024);
    memset(str_icounter, 0 , 1024);
    sprintf(str_icounter, "%d", icounter);
    if (g_hash_table_size((*conn)->commands_hash) == icounter){
	return 2;
    }
    char* nextcommand = g_hash_table_lookup((*conn)->commands_hash, str_icounter);
    int i = openproto_detect_command(nextcommand);
    switch (i){
	case OPENPROTO_WRITE:
	case OPENPROTO_WRITELN:
	case OPENPROTO_CLOSE:
	case OPENPROTO_DO:
	case OPENPROTO_DIE:
	    return 1;
	    break;
	default:
	    return 0;
    }
//    if ((i == OPENPROTO_WRITE) || (i == OPENPROTO_WRITELN) || (i == OPENPROTO_CLOSE)){
//	return 1;
//    }else{
//	return 0;
//    }
}

int openproto_next_read_command(struct connection **conn)
{
    int icounter = (*conn)->now_command;
    char* str_icounter = malloc(sizeof(char) * 1024);
    memset(str_icounter, 0 , 1024);
    sprintf(str_icounter, "%d", icounter);
    char* nextcommand = g_hash_table_lookup((*conn)->commands_hash, str_icounter);
    if (!nextcommand){
	debug("Command batch is empty");
    }else{

    }
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
    if (strpos(OPENPROTO_STR_WRITELN, string) == 0){
	return OPENPROTO_WRITELN;
    }else
    if (strpos(OPENPROTO_STR_READ, string) == 0){
	return OPENPROTO_READ;
    }else
    if (strpos(OPENPROTO_STR_DO, string) == 0){
	return OPENPROTO_DO;
    }else
    if (strpos(OPENPROTO_STR_DIE, string) == 0){
	return OPENPROTO_DIE;
    }else
    if (strpos(OPENPROTO_STR_EVENT, string) == 0){
	return OPENPROTO_EVENT;
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
    if (strlen(string) < (endEvent+2)){
	logger("Bad Comamnd Format, empty value", DEBUG_WARN);
	return 0;
    }
    j=0;
    char *str = malloc(sizeof(char) * (strlen(string) + 1));
    memcpy(str, string+(endEvent+2)*sizeof(char), strlen(string)+1);
    (*value) = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy((*value), str);

    free(str);
    return 1;
}

int openproto_run_CONNECT(char* uri, unsigned int event, int console_efd, GHashTable *send, GHashTable *recaive)
{
    trim(&uri);
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

    char* sock_str = malloc(128 * sizeof(char));
    sprintf(sock_str, "%d", sockfd);
    g_hash_table_insert(send, sock_str, "");
    g_hash_table_insert(recaive, sock_str, "");

    return sockfd;
}

void openproto_run_CLOSE(unsigned int event, int sockfd)
{
    debug("Close!");
    close(sockfd);
}

char* openproto_run_READ(unsigned int event, char* value, GHashTable *send, GHashTable *recaive, int sockfd)
{
    char* sock_str = malloc(128 * sizeof(char));
    sprintf(sock_str, "%d", sockfd);

    char* recaived = g_hash_table_lookup(recaive, sock_str);
    debug_s("> ", recaived);
    if (strpos("STRING",value) == 0){
	if (strpos("\n", recaived) == -1){
	    return (char*)-1;
	}
	//process string
	g_hash_table_insert(recaive, sock_str, "");
	return (char*)0;
    }else if(strpos("ALL",value) == 0){
	//next command is here command
	return (char*)-1;
    }
}

int openproto_run_WRITELN(char* value, int sockfd)
{
    char* newval = malloc(sizeof(char) * strlen(value) + 3*sizeof(char));
    memcpy(newval, value, sizeof(char)*strlen(value));
    unsigned int len = strlen(value);
    newval[len] = '\r';
    newval[len+1] = '\n';
    newval[len+2] = '\0';
    return openproto_run_WRITE(newval, sockfd);
}

int openproto_run_WRITE(char* value, int sockfd)
{
    char* sock_str = malloc(128 * sizeof(char));
    sprintf(sock_str, "%d", sockfd);
    debug_s("< ",value);
    int sended = send(sockfd, value, strlen(value), 0);
    if (sended < 0){
	logger("send returned -1 status", DEBUG_ERROR);
	if (errno == EWOULDBLOCK){
	    info("socket is in progress now....");
	    return -1;
	}
	return -2;
    }
    return sended;
}
