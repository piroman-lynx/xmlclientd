#include <netdb.h>

int url_parse(char* uri, int *proto, struct hostent **host, int *port);