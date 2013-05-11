#include "const.h"

#include <stdlib.h>

struct connection* init_connection()
{
    struct connection *conn = malloc(sizeof(struct connection));
    conn->commands_count = 0;
    conn->now_command = 0;
    conn->sockfd = 0;
    conn->efd = 0;
    conn->commands_hash  = g_hash_table_new(g_str_hash, g_str_equal);
    conn->recaive_hash  = g_hash_table_new(g_str_hash, g_str_equal);
    conn->send_hash  = g_hash_table_new(g_str_hash, g_str_equal);
    return conn;
}