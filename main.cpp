#include "util.hpp"
#include "def.h"
#include "sock.hpp"
#include "connect.h"


int main()
{
    int port = 8888;
    ignSigpipe();

    int server_fd = serverfdInit(port);
    if(server_fd == -1){
        fprintf(stderr, "serverfdInit is fail.\n");
        return 1;
    }
    if(setNonblocking(server_fd) == -1){
        fprintf(stderr, "setNonblocking is fail.\n");
        return 1;
    }
    if(epoll_add(server_fd, request, EPOLLIN) == -1){
        fprintf(stderr, "epoll_add() is fail.\n");
        return 1;
    }
    while(1){
        std::shared_ptr<ner_connect> k;
    }
    return 0;
}