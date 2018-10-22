#pragma once

#include "connect.h"
#include "sock.hpp"
#include "def.h"
#include <sys/epoll.h>
#include <memory>
#include <vector>

typedef std::shared_ptr<ner_connect> con_ptr;
//这里只设置了1024个FD支持;下一步可以写成json文件配置
const int MAX_FD = 1024;

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
    int ner_poll_wait(int server_fd, int max_events, int timeout);

    std::vector<con_ptr> getEvents(int server_fd, int nums);
}