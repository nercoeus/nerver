#ifndef SOCK_H_
#define SOCK_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "def.h"
#include "threadpool.h"
#include "epoll.h"
#include "util.h"
#include "http.h"

struct ThreadData{
    int ser_fd;
    int cli_fd;
    int epl_fd;
    void (*callback)(void *ev_arg);
};

int socket_init(int port)
{
    if (port < 1024 || port > 65535)
    {
        return -1;
    }

    int server_fd = 0;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        return -1;
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        return -1;
    }

    if (listen(server_fd, SERVER_SIZE) == -1)
    {
        return -1;
    }
    if (server_fd == -1)
    {
        close(server_fd);
        return -1;
    }

    return server_fd;
}

int setSockNoBlocking(int server_fd)
{
    int flag = fcntl(server_fd, F_GETFL, 0);
    if (flag == -1)
    {
        return -1;
    }
    flag = flag | O_NONBLOCK;
    if (fcntl(server_fd, F_SETFL, flag) < 0)
    {
        return -1;
    }
    return 0;
}

int sock_accept(int server_fd, struct sockaddr* client_addr, socklen_t* clilen){
    int client_fd = 0;
    while(1){
        client_fd = accept(server_fd, client_addr, clilen);
        if(client_fd == -1){
            if(errno != EAGAIN && errno != EPROTO && errno != EINTR && errno != ECONNABORTED)
            {
                exit(1);
            }
        }
        if(client_fd > 0){
            return client_fd;
        }
        else{
            continue;
        }
    }
}

void handle_func(void* data){

    ner_http *http_data = ner_http_init();

    epoll_event event;
    ThreadData* arg = (ThreadData *)data;
    int server_fd = arg->ser_fd;
    int client_fd = arg->cli_fd;
    int epoll_fd = arg->epl_fd;

    event.data.fd = client_fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_add(epoll_fd, client_fd, &event);

    int tid = pthread_self();
    fprintf(stderr, "thread %d runs\n", tid);
    char* buf = (char*)malloc(BUFSIZE);
    bzero(buf, BUFSIZE);

    int nread = 0;
    int n = 0;
    string http_str;
    while(1){
        n = read(client_fd, buf, sizeof(buf));
        if(n > 0){
            nread+=n;
            http_str += buf;
            bzero(buf, sizeof(buf));
        }
        else if(n == 0){
            break;
        }
        else if(-1 == n && errno == EINTR)
			continue;
		else if(-1 == n && errno == EAGAIN)
			break;
		else if(-1 == n && errno == EWOULDBLOCK)
		{
			perror("socket read timeout");
			goto out;
		}
		else
		{
			perror("read http request error");
			free(buf);
			break;
		}
    }
    if(0 < nread){
        std::string http_str(buf, buf + nread);
        std::cout<<http_str<<endl;

        //if(http_request(http_str, http_data));
    }

out:
    ner_http_free(http_data);
    //close(client_fd);
}

#endif