#include "util.hpp"
#include "def.h"
#include "sock.hpp"
#include "connect.h"
#include "epoll.h"
#include "threadpool.h"


int main()
{
    int port = 8888;
    ignSigpipe();

    std::shared_ptr<ner_epoll> epoll(new ner_epoll());
    if(epoll->epoll_init(EVENTS_SIZE) < 0){
        fprintf(stderr, "epoll_init is fail.\n");
        return 1;
    }

    ner_threadpool* pool;
    if((pool = threadpool_create(1)) == NULL){
        fprintf(stderr, "threadpool_init is fail.\n");
        return 1;
    }

    int server_fd = serverfdInit(port);
    if(server_fd == -1){
        fprintf(stderr, "serverfdInit is fail.\n");
        return 1;
    }
    if(setNonblocking(server_fd) == -1){
        fprintf(stderr, "setNonblocking is fail.\n");
        return 1;
    }
    
    con_ptr t_con(new ner_connect());
    t_con->setFd(server_fd);
    
    if(epoll->epoll_add(server_fd, t_con, EPOLLIN | EPOLLET) == -1){
        fprintf(stderr, "epoll_add() is fail.\n");
        return 1;
    }
    while(1){
        epoll->ner_poll_wait(server_fd, EVENTS_SIZE, -1);
    }
    return 0;
}