#ifndef CONNECT_H_
#define CONNECT_H_

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unordered_map>
#include "def.h"
#include "pthread.h"
#include "util.h"

struct ner_connect
{
private:
    int fd;
    int epoll_fd;
    int state;
    bool mark;
    int method;
    int http_ver;
    std::string content;
    std::string file_name;
    std::unordered_map<std::string, std::string> headers;
    
public:
    ner_connect();
    ner_connect(int _fd, int _epoll_fd);
    void handle();
    int getFd();
    void setFd(int fd);
    int parseURI();
    int parseHeader();
};

#endif