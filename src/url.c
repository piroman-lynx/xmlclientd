#include "url.h"

#include <netdb.h>
#include <stdlib.h>
#include <string.h>

int url_parse(char* uri, int *proto, struct hostent **host, int *port)
{
    if (strpos("tcp://",uri)==0){
	(*proto) = 1;
    }else{
	return -1;
    }
    int i,j=0;
    char* lhost = malloc(sizeof(char) * (strlen(uri)+1));
    for (i=6; (i<strlen(uri)&&(uri[i] != ':')); i++)
    {
	lhost[j]=uri[i];
	j++;
    }
    lhost[j]='\0';
    if (!((*host) = gethostbyname(lhost))){
	free(lhost);
	return -1;
    }
    free(lhost);
    i++;
    j=0;
    char* str_port = malloc(sizeof(char) * (strlen(uri) - i + 1));
    for (i=i; i<strlen(uri); i++){
	str_port[j] = uri[i];
	j++;
    }
    str_port[j]='\0';
    (*port) = atoi(str_port);
    free(str_port);
    if ((*port) < 1){
	return -1;
    }
    return 1;
}
