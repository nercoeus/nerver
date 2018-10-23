#include "connect.h"

ner_connect::ner_connect():
    keep_alive(false), read_or_write(EPOLL_IN)
{
}
ner_connect::ner_connect(int _epoll_fd, int _fd):
    epoll_fd(_epoll_fd), fd(_fd), keep_alive(false), read_or_write(EPOLL_IN)
{
}
ner_connect::~ner_connect()
{
}
int ner_connect::getFd()
{
    return fd;
}
void ner_connect::setFd(int _fd)
{
    fd = _fd;
}
void ner_connect::setStation(int s)
{
    read_or_write = s;
}
void ner_connect::handle()
{
    printf("hello");
}

