
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include "threadpool.h"
#include "sock.h"
#include "epoll.h"
#include "def.h"
#include "connect.h"

int main(int argc, char **argv)
{
    /*数据初始化*/
    int epoll_fd;
    ner_threadpool *pool;
    int server_fd, client_fd, sock_fd;
    struct sockaddr_in client_addr;
    char buf[MAXLINE];
    struct epoll_event event;
    struct epoll_event events[20];
    socklen_t clilen;
    ner_connect *connect_data;

    /*初始化epoll*/
    epoll_fd = epoll_init(SERVER_SIZE + 1);
    if (epoll_fd < 0)
    {
        return 1;
    }
    /*初始化线程池*/
    pool = threadpool_create(THREAD_SIZE);
    if (pool == NULL)
    {
        fprintf(stderr, "create pool is fail\n");
        exit(1);
    }
    /*创建套接字*/
    server_fd = socket_init(PORT);
    if (server_fd < 0)
    {
        perror("socket_init\n");
        return 1;
    }
    /*将监听套接字添加进epoll*/
    event.events = EPOLLIN | EPOLLET;
    connect_data = new ner_connect();
    connect_data->setFd(server_fd);
    cout<<server_fd<<endl;
    event.data.fd = server_fd;
    event.data.ptr = (void *)connect_data;
    epoll_add(epoll_fd, server_fd, &event);

    //设置套接字为非阻塞模式
    if (setSockNoBlocking(server_fd) < 0)
    {
        return 1;
    }
    while (1)
    {
        int nums = ner_epoll_wait(epoll_fd, events, MAXEVENTS, -1);
        printf("num : %d %d\n", nums, events[0].data.fd);
        if (nums == -1 && errno == EINTR)
        {
            continue;
        }

        for (int i = 0; i < nums; i++)
        {
            if (events[i].data.fd == server_fd)
            {
                cout<<"accept"<<endl;
                sock_fd = sock_accept(server_fd, (struct sockaddr *)&client_addr, &clilen);
                setSockNoBlocking(sock_fd);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = sock_fd;
                connect_data = new ner_connect(sock_fd, epoll_fd);
                event.data.ptr = (void *)connect_data;
                epoll_add(epoll_fd, sock_fd, &event);
            }
            else if (events[i].events & EPOLLIN)
            {
                printf("EPOLLIN\n");
                char buf[1024];
                read(events[i].data.fd, buf, 1024);
                cout << buf << endl;
                bzero(buf, 1024);
            }
            else if (events[i].events & EPOLLOUT)
            {
            }
        }

        //handle_events(epoll_fd, server_fd, events, nums, pool);
    }
    threadpool_destroy(pool, 1);

    close(server_fd);
    return 0;
}