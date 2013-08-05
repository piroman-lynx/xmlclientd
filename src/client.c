#include "process.h"
#include "debug.h"
#include "server.h"
#include "const.h"
#include "client.h"

#include <stdlib.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <glib.h>
#include <string.h>

//#define MAXEVENTS 1024

void client_thread(int argc, int* argv)
{
    logger("Worker process started: cleint", DEBUG_INFO);
    //todo: pass socket here
    if (argc > 0){
	set_epoll_on_shared_socket(argv[1]);
    }
    exit(0);
}

void rpush_to_buff(GHashTable* socket_recaive_hash, int socket, char* buff)
{
//    debug("push_to_buff!");

    char* str_socket = malloc(sizeof(char) * 128);
    sprintf(str_socket, "%d", socket);

    char* str = g_hash_table_lookup(socket_recaive_hash, str_socket);

    if (!str){
	str = malloc(sizeof(char) * 1024);
	memset(str, 0, (1024*sizeof(char)));
    }
    
    char* tmpstr = malloc(sizeof(char) * (strlen(str)+1));
    memcpy(tmpstr, str, sizeof(char) * (strlen(str)+1));
    //free(str); //TODO: Segmentation fault here
    //debug("free");
    str = malloc(sizeof(char) * (strlen(str)+strlen(buff)+1));
    memcpy(str, tmpstr, sizeof(char) * (strlen(str)+1));

    int oldlen = strlen(str);
    int i,j=0;
//    printf("oldlen: %d, < %d", oldlen, oldlen+strlen(buff));
    for (i=oldlen; i<oldlen+strlen(buff); i++){
	str[i] = buff[j];
	j++;
    }
    str[i] = '\0';
//    debug("free str_socket");
//    debug("dataiN:");
//    debug(str);
    g_hash_table_insert(socket_recaive_hash, str_socket, str);
    free(str_socket);
    free(tmpstr);
    return;
}


void client_start_epoll(struct connection **conn)
{
    char buf[1024];
    struct epoll_event *events;
    events = calloc (MAXEVENTS, sizeof (struct epoll_event));

    int icounter=(*conn)->now_command;

    printf("efd=%d\n", (*conn)->efd);

    while (1){
      int n, i;
      n = epoll_wait ((*conn)->efd, events, MAXEVENTS, -1);
      for (i = 0; i < n; i++){
	    //115 - operation in progress
            if ((errno != 115) && ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))) {
		perror("Epoll");
		printf("epoll errno=%d\n",errno);
		logger("epoll error!", DEBUG_ERROR);
		close (events[i].data.fd);
		continue;
	    }else{
	      printf("errno: %d\n", errno);
              int done = 0;
              while (1){
                  ssize_t count;
                  char buf[512];
		  memset(buf, 0, 512);
                  count = read (events[i].data.fd, buf, sizeof buf);
                  if (count == -1){
                      if (errno != EAGAIN){
                          perror ("read");
                          done = 1;
                      }
                      break;
                  } else if (count == 0) {
                      done = 1;
                      break;
                  } else {
//		    debug("Readed!:");
//		    debug(buf);
//		    printf("now fd in epoll: %d\n", events[i].data.fd);
		    rpush_to_buff((*conn)->recaive_hash, events[i].data.fd, buf);
//		    printf("run_next_read_command sockfd: %d\n",(*conn)->sockfd);
		    openproto_next_read_command(conn);
		    int res;
		    while (res = openproto_detect_write(conn) == 1){
			openproto_run_command("", conn);
		    }
		    if (res == 2){
			debug("Connection closed By script, exiting");
			break;
		    }
		  }
              }
	      if (done){
                  printf ("Closed connection on descriptor %d\n", events[i].data.fd);
                  close (events[i].data.fd);
              }
          }
      }
   }
}


int client_epoll_create()
{
    int efd = epoll_create1 (0);
    if (efd == -1)
    {
	logger("epoll_create1 failed",DEBUG_ERROR);
	return -1;
    }
    client_start_watcher();
    return efd;
}

void client_start_process(int argc, int* argv)
{
    int result = process_make_new(&client_thread, argc, argv);
    if (result < 0){
	debug("can't fork client\n");
    }
}

GHashTable* watchers;



void client_watcher_add(struct connection **conn)
{
    char *sfm = malloc(sizeof(char)*64);
    //char *connf = malloc(sizeof(char)*64);
    int sfd = g_hash_table_size(watchers);
    wait_watcher_lock_and_lock();
    sprintf(sfm, "%d", sfd);
    //sprintf(connf, "%d", conn);
    g_hash_table_insert(watchers, sfm, conn);
    wait_watcher_lock_unlock();
}

void* client_watcher_entry_point()
{
    watchers = g_hash_table_new(g_str_hash, g_str_equal);
    char *srm = malloc(sizeof(char)*64);
    int i;
    while (1){
	wait_watcher_lock_and_lock();
	int size=g_hash_table_size(watchers);
	if (size > 0){
	    wait_watcher_lock_unlock();
	    for (i=0; i<size; i++){
		sprintf(srm, "%d", i);
		struct connection **conn = g_hash_table_lookup(watchers, srm);
		printf("Watcher check: %s, conn=%d\n", srm, conn);
		if (openproto_detect_write(conn) == 1){
		    //check
		    debug("Watcher wait write!");
		    while (openproto_detect_write(conn) == 1){
		    debug("Watcher do write!");
			openproto_run_command("", conn);
		    }
		    wait_watcher_lock_and_lock();
		    //удаляем из watcher-a
		    wait_watcher_lock_unlock();
		}else{
		    wait_watcher_lock_and_lock();
		    //удаляем из watcher-a
		    wait_watcher_lock_unlock();
		}
	    }
	}
	sleep(1);
    }
}

void wait_watcher_lock_and_lock()
{
//	debug("Temporary not implemented");
}

void wait_watcher_lock_unlock()
{
//	debug("Temporary not implemented");
}

void client_start_watcher()
{
    pthread_t watcher_pt;
    pthread_create(&watcher_pt, NULL, client_watcher_entry_point, NULL);
    return;
}

