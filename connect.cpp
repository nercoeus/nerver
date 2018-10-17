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
        cout << "content: \n"
             << content << endl;

        if (state == STATE_PARSE_URI)
        {
            int flag = this->parseURI();
            if (flag == PARSER_AGAIN)
            {
                break;
            }
            else if (flag == PARSER_ERROR)
            {
                perror("parse uri is fail");
                break;
            }
        }
        if (state == STATE_PARSE_HEADERS)
        {
            int flag = this->parseHeader();
            if (flag == PARSER_AGAIN)
            {
                break;
            }
            else if (flag == PARSER_ERROR)
            {
                perror("parse uri is fail");
                break;
            }
            if(method == METHOD_POST)
            {
                state = STATE_RECV_BODY;
            }
            else 
            {
                state = STATE_ANALYSIS;
            }
        }
        if(state == STATE_RECV_BODY){
            //
        }
        /*
        if (state == STATE_ANALYSIS)
        {
            int flag = this->analysisRequest();
            if (flag < 0)
            {
                isError = true;
                break;
            }
            else if (flag == ANALYSIS_SUCCESS)
            {

                state = STATE_FINISH;
                break;
            }
            else
            {
                isError = true;
                break;
            }
        }
        */
    }
}


int ner_connect::parseURI()
{
    string str = content;

    int pos = str.find('\r', mark);
    if (pos < 0)
    {
        return PARSER_AGAIN;
    }

    string uri_str = str.substr(0, pos);
    cout << "URI: " << uri_str << endl;
    if (str.size() > pos + 1)
    {
        str = str.substr(pos + 2);
        content = str;
    }
    else
    {
        str.clear();
    }
    cout << "uri_str: " << uri_str << endl;
    pos = uri_str.find("GET");
    if (pos < 0)
    {
        pos = uri_str.find("POST");
        if (pos < 0)
        {
            return PARSER_ERROR;
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
        return PARSER_ERROR;
    }
    else
    {
        int _pos = uri_str.find(' ', pos);
        if (pos < 0)
            return PARSER_ERROR;
        else
        {
            if (_pos - pos > 1)
            {
                file_name = uri_str.substr(pos + 1, _pos - pos - 1);
                int _pos2 = file_name.find('?');
                if (_pos2 >= 0)
                {
                    file_name = file_name.substr(0, _pos2);
                }
            }
            else
            {
                file_name = "index.html";
            }
        }
        pos = _pos;
    }
    cout << "file_name : " << file_name << endl;
    pos = uri_str.find('/', pos);
    if (pos < 0)
        return PARSER_ERROR;
    else
    {
        if (uri_str.size() - pos <= 3)
        {
            return PARSER_ERROR;
        }
        else
        {
            string ver = uri_str.substr(pos + 1, 3);
            if (ver == "1.0")
            {
                http_ver = HTTP_1_0;
            }
            else if (ver == "1.1")
            {
                http_ver = HTTP_1_1;
            }
            else
            {
                return PARSER_ERROR;
            }
        }
    }
    cout << "http version : " << http_ver << endl;
    state = STATE_PARSE_HEADERS;
    cout<<"content : "<<content<<endl;
    return PARSER_SUCCESS;
}

int ner_connect::parseHeader()
{
    string header = content;
    int pos = 0;
    for(int i = 0;i + 1 < header.size();){
        if(header[i] == '\r' && header[i+1] == '\n'){
            pos+=2;
            cout<<"parse header end"<<endl;
            break;
        }
        else if(header[i] == '\n'){
            pos = ++i;
            continue;
        }
        else {
            int pos1 = header.find(':', pos);
            if(pos < 0){
                return PARSER_ERROR;
            }
            else {
                int pos2 = header.find('\r', pos);
                if(pos2 < 0){
                    return PARSER_ERROR;
                }
                else{
                    string key = header.substr(pos, pos1 - pos);
                    string value = header.substr(pos1+2, pos2-pos1-2);
                    headers[key] = value;
                    i = pos2+1;
                }
            }
            pos = i;
        }
    }
    content = header.substr(pos);
    cout<<content<<endl;
    cout<<"header : **************************************8"<<endl;
    for(auto temp : headers){
        cout<<temp.first<<"@:@ "<<temp.second<<endl;
    }
    cout<<"header : **************************************8"<<endl;
    return PARSER_SUCCESS;
}