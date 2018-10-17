#include "connect.h"
#include <iostream>
using namespace std;
ner_connect::ner_connect()
    : fd(0), epoll_fd(0)
{
}

ner_connect::ner_connect(int _fd, int _epoll_fd)
    : fd(_fd), epoll_fd(_epoll_fd)
{
}

void ner_connect::setFd(int _fd)
{
    this->fd = _fd;
}

int ner_connect::getFd()
{
    return this->fd;
}

void ner_connect::handle()
{
    printf("handle %lu\n", pthread_self());
    char buf[MAXLINE];
    printf("handle on sockfd : %d\n", fd);
    while (1)
    {
        int nread = readn(fd, buf, MAXLINE);
        if(nread < 0){
            printf("read is error\n");
            break;
        }
        else if (nread == 0){
            printf("nread = 0\n");
            break;
        }
        cout<<"nread: "<<nread<<endl;
        std::string http_str(buf, buf + nread-1);
        cout<<"str: "<<http_str<<endl;
        this->content += http_str;
        cout<<"content: "<<content<<endl;
    }
}