#include "process.h"
#include "debug.h"
#include "server.h"

#include <stdlib.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <errno.h>
#include <glib.h>
#include <string.h>

#define MAXEVENTS 1024

void client_thread(int argc, int* argv)
{
    logger("Worker process started: cleint", DEBUG_INFO);
    //todo: pass socket here
    if (argc > 0){
	set_epoll_on_shared_socket(argv[1]);
    }
    exit(0);
}

void client_start_epoll(int efd, GHashTable* socket_send_hash, GHashTable* socket_recaive_hash, GHashTable* commands_hash, int command_count, int ecounter)
{
    char buf[1024];
    struct epoll_event *events;
    events = calloc (MAXEVENTS, sizeof (struct epoll_event));

    int icounter=ecounter;

    printf("efd=%d\n", efd);

    while (1){
      int n, i;
      n = epoll_wait (efd, events, MAXEVENTS, -1);
      for (i = 0; i < n; i++){
	    //115 - operation in progress
            if ((errno != 115) && ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))) {
		perror("Epoll");
		printf("epoll errno=%d\n",errno);
		logger("epoll error!", DEBUG_ERROR);
		close (events[i].data.fd);
		continue;
	    }else{
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
		    debug("Readed!:");
		    debug(buf);
		    icounter = push_to_buff(socket_recaive_hash, command_count, icounter, commands_hash, buf);
		  }
              }if (done){
                  printf ("Closed connection on descriptor %d\n", events[i].data.fd);
                  close (events[i].data.fd);
              }
          }
      }
   }
}

int push_to_buff(GHashTable* socket_recaive_hash, int command_count, int icounter, GHashTable*  commands_hash, char* buff)
{
    debug("push_to_buff!");
    char* rbuff = malloc(sizeof(char) * (strlen(buff)+1));
    char* oldrbuff = rbuff;
    memcpy(rbuff, buff, sizeof(char) * (strlen(buff)+1));
    int have_data = 1;
    char* str_icounter = malloc(sizeof(char) * 128);
    while (have_data){
        sprintf(str_icounter, "%d", icounter);
	char* str = g_hash_table_lookup(socket_recaive_hash, str_icounter);
	if (!str){
	    str = malloc(sizeof(char)*1024);
	}
	
	int oldlen = strlen(str);
	int i,j=0;
	for (i=oldlen; i<strlen(rbuff); i++){
	    if ((rbuff[j]!='\n')&&(rbuff[j]!='\r')){
	        str[i] = buff[j];
	    }else{
		//next string
		if ((rbuff[j+1]=='\r')||(rbuff[j+1]=='\n')){
		    j+= 2;
		}else{
		    j++;
		}
		sprintf(str_icounter, "%d", atoi(str_icounter) + 1);
		if (j<strlen(rbuff)){
		    rbuff = rbuff + (sizeof(char) * j); //move pointer
		    debug("String partialy writed!");
		    have_data=1;
		    break; //goto while(have_data)
		}else{
		    have_data=0; //go out
		    debug("String writed!");
		    break;
		}
	    }
	    j++;
	    str[i]='\0';
	}
	debug(str);
    }
    free(oldrbuff);
    debug("oldrbuff free");
    int ret = atoi(str_icounter);
    free(str_icounter);
    debug("str_icounter free");
    return ret;
}

int client_epoll_create()
{
    int efd = epoll_create1 (0);
    if (efd == -1)
    {
	logger("epoll_create1 failed",DEBUG_ERROR);
	return -1;
    }
    return efd;
}

void client_start_process(int argc, int* argv)
{
    int result = process_make_new(&client_thread, argc, argv);
    if (result < 0){
	debug("can't fork client\n");
    }
}