#pragma once

#include "connect.h"
#include "sock.h"
#include "threadpool.h"
#include <sys/epoll.h>
#include <memory>
#include <vector>
#include <stdio.h>

typedef std::shared_ptr<ner_connect> con_ptr;


class ner_epoll
{
public:

private:
    epoll_event *events;
    int epoll_fd;
    con_ptr fd_con[MAX_FD];

public:
    int epoll_init(int listen_num);
    int epoll_add(int fd, con_ptr con, uint32_t events);
    int epoll_mod(int fd, con_ptr con, uint32_t events);
    int epoll_del(int fd, con_ptr con, uint32_t events);
    int ner_poll_wait(int server_fd, int max_events, int timeout, ner_threadpool* pool);

    std::vector<con_ptr> getEvents(int server_fd, int nums);
};