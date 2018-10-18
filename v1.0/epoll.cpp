#include "epoll.h"

//struct epoll_event* events;

int epoll_init(int flags){
    int epoll_fd = epoll_create(flags);
    if(epoll_fd < 0){
        return -1;
    }
    //events = new epoll_event[MAXEVENTS];
    return epoll_fd;
}

int epoll_add(int epoll_fd, int fd, struct epoll_event* event){
    int res = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, event);
    if(res < 0){
        return -1;
    }
    return 0;
}

int epoll_mod(int epoll_fd, int fd, struct epoll_event* event){
    int res = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, event);
    if(res < 0){
        return -1;
    }
    return 0;
}

int epoll_del(int epoll_fd, int fd, struct epoll_event* event){
    int res = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, event);
    if(res < 0){
        return -1;
    }
    return 0;
}

int ner_epoll_wait(int epoll_fd, struct epoll_event *events, int max_events, int timeout){
    int res = epoll_wait(epoll_fd, events, max_events, timeout);
    if(res < 0){
        return -1;
    }
    return res;
}