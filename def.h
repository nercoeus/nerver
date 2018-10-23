#pragma once

enum EPOLL_STATION{
    EPOLL_IN = 1,
    EPOLL_OUT = 2
};
const int SERVER_SIZE = 1024;
//这里只设置了1024个FD支持;下一步可以写成json文件配置
const int MAX_FD = 1024;
//epoll_wait等待队列的大小
const int EVENTS_SIZE = 1024;
