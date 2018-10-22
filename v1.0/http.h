#ifndef HTTP_H_
#define HTTP_H_

#include <string.h>
#include <string>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <queue>
#include <iostream>
#include "def.h"
#include "util.h"

using namespace std;


class ner_connect;
struct nerTimer;
struct timerCmp;



//用来进行超时的管理,如果超时就断开连接
struct nerTimer
{
    bool deleted;               //是否断开连接
    size_t expired_time;        //期望断开时间,这里根据系统时间计算得来
    ner_connect *connect_data;  //链接的函数

    nerTimer(ner_connect *connect_data, int timeout);
    ~nerTimer();
    void update(int timeout);     //更新时间,即使就是重新设置一下时间即可
    bool isvalid();
    void clearReq();              //断开链接时清除
    void setDeleted();            //设置断开链接
    bool isDeleted() const;       //是否超时
    size_t getExpTime() const;    //获取期望时间
};
//超时队列的比较参数
struct timerCmp
{
    bool operator()(const nerTimer *a, const nerTimer *b) const;
};

ssize_t readn(int fd, void* buf, size_t n);
ssize_t writen(int fd, void* buf, size_t n);



typedef map<string, string>   ner_http_header;

typedef struct{
    string method;
    string url;
    string version;

    ner_http_header header;
    string body;
}ner_http;

ner_http* ner_http_init();
int http_request(string http_str, ner_http* http_data);
void ner_http_free(ner_http* http_data);
void httpError404(int fd);



#endif