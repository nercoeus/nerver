#ifndef EPOLL_H_
#define EPOLL_H_

#include <sys/epoll.h>
#include "def.h"

int epoll_init(int flags);
int epoll_add(int epoll_fd, int fd, struct epoll_event *event);
int epoll_mod(int epoll_fd, int fd, struct epoll_event *event);
int epoll_del(int epoll_fd, int fd, struct epoll_event *event);
int ner_epoll_wait(int epoll_fd, struct epoll_event *event, int max_events, int timeout);

#endif