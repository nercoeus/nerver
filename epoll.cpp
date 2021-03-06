#include "epoll.h"


void nerHandle(std::shared_ptr<void> req)
{
    std::shared_ptr<ner_connect> t_con = std::static_pointer_cast<ner_connect>(req);
    t_con->handle();
}

int ner_epoll::epoll_init(int epoll_size)
{
    epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
    {
        perror("epoll_create is fail\n");
        return -1;
    }
    events = new epoll_event[epoll_size];
    return 0;
}

int ner_epoll::epoll_add(int fd, con_ptr con, uint32_t event)
{
    epoll_event _event;
    _event.data.fd = fd;
    _event.events = event;
    fd_con[fd] = con;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &_event) < 0)
    {
        perror("epoll_add is fail\n");
        return -1;
    }
    return 0;
}

int ner_epoll::epoll_mod(int fd, con_ptr con, uint32_t event)
{
    epoll_event _event;
    _event.data.fd = fd;
    _event.events = event;
    fd_con[fd] = con;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &_event) < 0)
    {
        perror("epoll_mod is fail\n");
        fd_con[fd].reset();
        return -1;
    }
    return 0;
}

int ner_epoll::epoll_del(int fd, con_ptr con, uint32_t event)
{
    epoll_event _event;
    _event.data.fd = fd;
    _event.events = event;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &_event) < 0)
    {
        perror("epoll_del is fail\n");
        return -1;
    }
    fd_con[fd].reset();
    return 0;
}

int ner_epoll::ner_poll_wait(int server_fd, int max_events,
                             int timeout, ner_threadpool *pool)
{
    int nums = epoll_wait(epoll_fd, events, max_events, timeout);
    //这里的events已经剔除了连接事件,仅仅处理输入输出事件.
    std::vector<con_ptr> cons = getEvents(server_fd, nums);
    if (cons.size() > 0)
    {
        for (auto &con : cons)
        {
            int flag;
            if((flag = threadpool_add(pool, nerHandle, con)) < 0){
                break;
            }
        }
    }
}

std::vector<con_ptr> ner_epoll::getEvents(int fd, int nums)
{
    std::vector<con_ptr> res;
    for (int i = 0; i < nums; i++)
    {
        int t_fd = events[i].data.fd;

        if (t_fd == fd)
        {
            int acc_fd = acceptConnect(fd);
            con_ptr t_ptr(new ner_connect());
            uint32_t _events = EPOLLIN | EPOLLET | EPOLLONESHOT;
            epoll_add(acc_fd, t_ptr, _events);
            //超时设置
        }
        else
        {
            // 排除错误事件
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP))
            {
                printf("error event\n");
                //if (fd_con[fd])
                //    fd_con[fd]->seperateTimer();
                fd_con[fd].reset();
                continue;
            }

            con_ptr t_con = fd_con[fd];
            if (t_con)
            {
                if ((events[i].events & EPOLLIN) || (events[i].events & EPOLLPRI))
                {
                    t_con->setStation(EPOLL_IN);
                }
                else
                {
                    t_con->setStation(EPOLL_OUT);
                }
                res.push_back(t_con);
                fd_con[fd].reset();
            }
            else
            {
                fprintf(stderr, "get fd error\n");
            }
        }
    }
    return res;
}