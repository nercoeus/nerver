#ifndef CONNECT_H_
#define CONNECT_H_

struct ner_connect
{
private:
    int fd;
    int epoll_fd;
public:
    ner_connect();
    ner_connect(int _fd, int _epoll_fd);
    void handle();
    void setFd(int fd);
};

#endif