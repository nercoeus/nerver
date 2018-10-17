#ifndef CONNECT_H_
#define CONNECT_H_

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include "def.h"
#include "pthread.h"
#include "util.h"

struct ner_connect
{
private:
    int fd;
    int epoll_fd;
    std::string content;
public:
    ner_connect();
    ner_connect(int _fd, int _epoll_fd);
    void handle();
    int getFd();
    void setFd(int fd);
};

#endif