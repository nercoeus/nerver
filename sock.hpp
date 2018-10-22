#pragma once

#include "util.h"
#include "def.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <stdio.h>

int serverfdInit(int port)
{
    if (port <= 1024 || port > 65535)
    {
        fprintf(stderr, "port numbet is error\n");
        return -1;
    }
    else
    {
        int flag = 0;
        int server_fd = 0;
        flag = socket(AF_INET, SOCK_STREAM, 0);
        if (flag == -1)
        {
            fprintf(stderr, "socket() fail\n");
            return -1;
        }
        struct sockaddr_in server_addr;
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            fprintf(stderr, "bind() fail\n");
            return -1;
        }
        if (listen(server_fd, SERVER_SIZE) == -1)
        {
            fprintf(stderr, "socket() fail\n");
            return -1;
        }
        if (server_fd == -1)
        {
            close(server_fd);
            return -1;
        }
        return server_fd;
    }
}

int acceptConnect(int fd)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t len = sizeof(addr);
    int res = 0;
    while ((res = accept(fd, (sockaddr *)&addr, &len)) > 0)
    {
        if (res < 0)
        {
            fprintf(stderr, "accept() fail\n");
            return -1;
        }
        if (res > MAX_FD)
        {
            close(res);
            fprintf(stderr, "accept fd is to large\n");
            return -1;
        }
        if (setNonblocking(res) < 0)
        {
            fprintf(stderr, "set nonblock is fail\n");
            return -1;
        }
        return res;
    }
    return -1;
}