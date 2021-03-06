
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <queue>
#include "threadpool.h"
#include "sock.h"
#include "epoll.h"
#include "def.h"
#include "connect.h"
#include "http.h"

extern pthread_mutex_t qlock;
extern priority_queue<nerTimer *, deque<nerTimer *>, timerCmp> nerTimeQueue;

void ner_handle(void *arg)
{
    ner_connect *con = (ner_connect *)arg;
    con->handle();
}

int main(int argc, char **argv)
{
    /*数据初始化*/
    int epoll_fd;
    ner_threadpool *pool;
    int server_fd, client_fd, sock_fd;
    struct sockaddr_in client_addr;
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
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    connect_data = new ner_connect();
    connect_data->setFd(server_fd);
    cout << server_fd << endl;
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
        cout << "nums: " << nums << " fd: " << server_fd << endl;
        if (nums == -1 && errno == EINTR)
        {
            continue;
        }

        for (int i = 0; i < nums; i++)
        {
            ner_connect *tconnect = (ner_connect *)(events[i].data.ptr);
            sock_fd = tconnect->getFd();
            if (sock_fd == server_fd)
            {
                client_fd = sock_accept(server_fd, (struct sockaddr *)&client_addr, &clilen);
                setSockNoBlocking(client_fd);
                event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                connect_data = new ner_connect(client_fd, epoll_fd);
                event.data.ptr = (void *)connect_data;
                printf("add new sockfd : %d\n", client_fd);
                epoll_add(epoll_fd, client_fd, &event);
                nerTimer *mtimer = new nerTimer(connect_data, 500);
                connect_data->setTimer(mtimer);
                nerMutexLock();
                nerTimeQueue.push(mtimer);
            }
            else
            {
                if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN)))
                {
                    printf("error event\n");
                    continue;
                }
                tconnect->seperateTimer();
                threadpool_add(pool, ner_handle, events[i].data.ptr);
            }
        }
        nerMutexLock();
        while (!nerTimeQueue.empty())
        {
            nerTimer *ptimer_now = nerTimeQueue.top();
            if (ptimer_now->isDeleted())
            {
                nerTimeQueue.pop();
                delete ptimer_now;
            }
            else if (ptimer_now->isvalid() == false)
            {
                nerTimeQueue.pop();
                delete ptimer_now;
            }
            else
            {
                break;
            }
        }
        

        //handle_events(epoll_fd, server_fd, events, nums, pool);
    }
    threadpool_destroy(pool, 1);

    close(server_fd);
    return 0;
}