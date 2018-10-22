#pragma once

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

static void signalHandler(int nSigno)
{
    signal(nSigno, signalHandler);
    switch (nSigno)
    {
    case SIGPIPE:
        printf("Process will not exit\n");
        break;
    default:
        printf("%d signal unregister\n", nSigno);
        break;
    }
}
static void ignSigpipe()
{
    signal(SIGPIPE, &signalHandler);
}

int setNonblocking(int fd)
{
    //获取套接字选项值
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
    {
        perror("use fcntl getfl is fail.\n");
        return -1;
    }
    //设置套接字选项值
    flag |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flag) == -1)
    {
        perror("use fcntl setfl is fail.\n");
        return -1;
    }
    return 0;
}