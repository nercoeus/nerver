#include "connect.h"
#include <iostream>
using namespace std;
ner_connect::ner_connect()
    : fd(0), epoll_fd(0), state(STATE_PARSE_URI), mark(0), method(0)
{
}

ner_connect::ner_connect(int _fd, int _epoll_fd)
    : fd(_fd), epoll_fd(_epoll_fd), state(STATE_PARSE_URI), mark(0), method(0)
{
}

void ner_connect::setFd(int _fd)
{
    this->fd = _fd;
}

int ner_connect::getFd()
{
    return this->fd;
}

void ner_connect::handle()
{
    printf("handle %lu\n", pthread_self());
    char buf[MAXLINE];
    printf("handle on sockfd : %d\n", fd);
    while (1)
    {
        int nread = readn(fd, buf, MAXLINE);
        if (nread < 0)
        {
            printf("read is error\n");
            break;
        }
        else if (nread == 0)
        {
            printf("nread = 0\n");
            break;
        }
        std::string http_str(buf, buf + nread - 1);
        this->content += http_str;
        cout << "content: \n" << content << endl;

        if (state == STATE_PARSE_URI)
        {
            int flag = this->parseURI();
        }
    }
}

int ner_connect::parseURI()
{
    string str = content;

    int pos = str.find('\r', mark);
    if (pos < 0)
    {
        return -1;
    }

    string uri_str = str.substr(0, pos);
    cout << "URI: " << uri_str << endl;
    if (str.size() > pos + 1)
    {
        str = str.substr(pos);
    }
    else
    {
        str.clear();
    }
    cout<<"uri_str: "<<uri_str<<endl;
    pos = uri_str.find("GET");
    if (pos < 0)
    {
        pos = uri_str.find("POST");
        if (pos < 0)
        {
            return -1;
        }
        else
        {
            method = METHOD_POST;
        }
    }
    else
    {
        method = METHOD_GET;
    }
    pos = uri_str.find("/", pos);
    if (pos < 0)
    {
        return -1;
    }
    else
    {
        int _pos = uri_str.find(' ', pos);
        if(pos < 0)
            return -1;
        else{
            if(_pos - pos > 1)
            {
                file_name = uri_str.substr(pos+1, _pos - pos - 1);
                int _pos2 = file_name.find('?');
                if(_pos2 >= 0){
                    file_name = file_name.substr(0, _pos2);
                }
            }
            else{
                file_name = "index.html";
            }
        }
        pos = _pos;
    }
    cout<<"file_name : "<<file_name<<endl;
    pos = uri_str.find('/', pos);
    if(pos < 0)
        return -1;
    else {
        if(uri_str.size()-pos <= 3){
            return -1;
        }
        else {
            string ver = uri_str.substr(pos+1, 3);
            if(ver == "1.0"){
                http_ver = HTTP_1_0;
            }
            else if(ver == "1.1"){
                http_ver = HTTP_1_1;
            }
            else{
                return -1;
            }
        }
    }
    cout<<"http version : "<<http_ver<<endl;
    state = STATE_PARSE_HEADERS;
    return 1;
}