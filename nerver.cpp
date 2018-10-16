
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include "threadpool.h"
#include "sock.h"
#include "epoll.h"
#include "def.h"

int main(int argc, char** argv)
{
    /*数据初始化*/
    int epoll_fd;
    ner_threadpool *pool;
    int server_fd;
    int client_fd;
    int sock_fd;
    struct sockaddr_in client_addr;
    char buf[MAXLINE];
    struct epoll_event event;
    struct epoll_event events[20];
    int nums;
    socklen_t clilen;
    ThreadData arg_data;

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
    event.data.fd = server_fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_add(epoll_fd, server_fd, &event);

    //设置套接字为非阻塞模式
    if (setSockNoBlocking(server_fd) < 0)
    {
        return 1;
    }
    while (1)
    {
        nums = ner_epoll_wait(epoll_fd, events, MAXEVENTS, -1);
        printf("num : %d\n", nums);
        if (nums == -1 && errno == EINTR)
        {
            continue;
        }

        for (int i = 0; i < nums; i++)
        {
            if (events[i].data.fd == server_fd)
            {
                sock_fd = sock_accept(server_fd, (struct sockaddr *)&client_addr, &clilen);
                setSockNoBlocking(sock_fd);
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = sock_fd;
                epoll_add(epoll_fd, sock_fd, &event);
            }
            else
            {
            }
        }

        //handle_events(epoll_fd, server_fd, events, nums, pool);
    }
    threadpool_destroy(pool, 1);

    close(server_fd);
    return 0;
}