
/*
* do accepts under epoll for server incoming connections
*/
void set_epoll_on_shared_socket(int socket);

/*
* called after accept
* runs:
* detect_proto()
* swtich ( run_xml_proto, run_open_proto  )
*/
void process_connection(int csocket);
