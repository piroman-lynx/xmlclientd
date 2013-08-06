
#ifndef OPENPROTO_EVENT_H
#define OPENPROTO_EVENT_H 1

/**
* вызвать обработку события
* eStatus:
* - Match: 0:Match, 1:DontMatch
* - Connect: 0:unreachable, 1:timeout
* - Read: 0:readTimeout, 1:connectionClosed
* - Write: 0:connectionClosed
* - Close: 0:alreadyClosed
*
* Возвраты:
* 0  - продолжить выполнение
* -1 - прервать выполнение
**/
int openproto_event_process(struct connection *conn, unsigned char eStatus);

/**
* Аналогично detect-write
**/
void openproto_event_detect_handler(struct connection *conn);

/**
* Аналгично выполнению коннекта
**/
void openproto_event_run_handler(struct connection *conn);

/**
* Низкоуровневое добавление
**/
void openproto_event_add_handler(struct connection *conn, int step, unsigned char eStatus);

#endif