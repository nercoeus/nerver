#ifndef CONNECT_H_
#define CONNECT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unordered_map>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "def.h"
#include "pthread.h"
#include "util.h"
#include "http.h"
#include "epoll.h"

typedef struct
{
	const char *type;
	const char *value;
}mime_node;

struct ner_connect
{
private:
    int fd;
    int epoll_fd;
    int state;
    bool mark;
    int method;
    int http_ver;
    std::string content;
    std::string file_name;
    bool keep_alive;
    std::unordered_map<std::string, std::string> headers;

public:
    ner_connect();
    ner_connect(int _fd, int _epoll_fd);
    void handle();
    int getFd();
    void setFd(int fd);
    int parseURI();
    int parseHeader();
    int httpConnect();
};

#endif