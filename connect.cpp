#include "connect.h"

ner_connect::ner_connect()
    :fd(0),epoll_fd(0)
{
}

ner_connect::ner_connect(int _fd, int _epoll_fd)
    :fd(_fd),epoll_fd(_epoll_fd)
{
}

void ner_connect::setFd(int _fd){
    this->fd = _fd;
}