#pragma once

#include "def.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <stdio.h>


class ner_connect : public std::enable_shared_from_this<ner_connect>
{
  private:
    int fd;
    int epoll_fd;
    int method;
    int state;
    bool keep_alive;
    int read_or_write;
    std::unordered_map<std::string, std::string> headers;

  public:
    ner_connect();
    ner_connect(int _epoll_fd, int fd);
    ~ner_connect();
    int getFd();
    void setFd(int _fd);
    void setStation(int s);
    void handle();
};

