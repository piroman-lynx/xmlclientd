#include <glib.h>

#ifndef CONST_H
#define CONST_H

#define PERMANENT_CYCLE 1
#define READ_BUFF_SIZE  1024
//#define SOMAXCONN 5000
#define MAXEVENTS 5000

struct connection {
    GHashTable* commands_hash;
    GHashTable* recaive_hash;
    GHashTable* send_hash;
    int commands_count;
    int now_command;
    int sockfd;
    int efd;
};

struct connection* init_connection();

#endif