#include "http.h"


ner_http *ner_http_init()
{
    ner_http *res = (ner_http *)new ner_http;
    if (res == NULL)
    {
        exit(-1);
    }
    return res;
}

int http_request(string http_str, ner_http *http_data)
{
    return 1;
}

void ner_http_free(ner_http *http_data)
{
    if (http_data == NULL)
    {
        return;
    }
    delete http_data;
}

void httpError404(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    struct stat sbuf;
    string file404 = "web/404.html";
    if (stat(file404.c_str(), &sbuf) < 0)
    {
        return ;
    }
    int src_fd = open(file404.c_str(), 00, 0);
    char *src_addr = static_cast<char *>(mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0));
    close(src_fd);

    // 发送文件并校验完整性
    int send_len = writen(client, src_addr, sbuf.st_size);
    if (send_len != sbuf.st_size)
    {
        perror("Send file failed");
        return ;
    }
    munmap(src_addr, sbuf.st_size);
}


nerTimer::nerTimer(ner_connect *connect_data, int timeout): 
    deleted(false), connect_data(connect_data)
{
    //cout << "nerTimer()" << endl;
    struct timeval now;
    gettimeofday(&now, NULL);
    // 以毫秒计
    expired_time = ((now.tv_sec * 1000) + (now.tv_usec / 1000)) + timeout;
}

nerTimer::~nerTimer()
{
    cout << "~nerTimer()" << endl;
    if (connect_data != NULL)
    {
        cout << "connect_data = " << connect_data << endl;
        delete connect_data;
        connect_data = NULL;
    }
}

void nerTimer::update(int timeout)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    expired_time = ((now.tv_sec * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool nerTimer::isvalid()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    size_t temp = ((now.tv_sec * 1000) + (now.tv_usec / 1000));
    if (temp < expired_time)
    {
        return true;
    }
    else
    {
        this->setDeleted();
        return false;
    }
}

void nerTimer::clearReq()      //这里并没有析构connect_data
{
    connect_data = NULL;
    this->setDeleted();
}

void nerTimer::setDeleted()
{
    deleted = true;
}

bool nerTimer::isDeleted() const
{
    return deleted;
}

size_t nerTimer::getExpTime() const
{
    return expired_time;
}

bool timerCmp::operator()(const nerTimer *a, const nerTimer *b) const
{
    return a->getExpTime() > b->getExpTime();
}