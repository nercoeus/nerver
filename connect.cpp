#include "connect.h"
#include <iostream>
#include <sys/stat.h>
using namespace std;

pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;
priority_queue<nerTimer*, deque<nerTimer*>, timerCmp> nerTimeQueue;

unordered_map<std::string, std::string> ner_mime =
    {
        {".html", "text/html"},
        {".xml", "text/xml"},
        {".xhtml", "application/xhtml+xml"},
        {".txt", "text/plain"},
        {".rtf", "application/rtf"},
        {".pdf", "application/pdf"},
        {".word", "application/msword"},
        {".png", "image/png"},
        {".gif", "image/gif"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".au", "audio/basic"},
        {".mpeg", "video/mpeg"},
        {".mpg", "video/mpeg"},
        {".avi", "video/x-msvideo"},
        {".gz", "application/x-gzip"},
        {".tar", "application/x-tar"}};

inline string ner_mime_type2value(string type)
{
    for (unordered_map<string, string>::iterator i = ner_mime.begin(); i != ner_mime.end(); ++i)
    {
        if (type == i->first)
            return i->second;
    }
    return NULL;
}

ner_connect::ner_connect()
    : fd(0), epoll_fd(0), state(STATE_PARSE_URI), mark(0), method(0)
    , file_name("web/"), timer(NULL)
{
}

ner_connect::ner_connect(int _fd, int _epoll_fd)
    : fd(_fd), epoll_fd(_epoll_fd), state(STATE_PARSE_URI), mark(0), method(0)
    , file_name("web/"), timer(NULL)
{
}

ner_connect::~ner_connect(){
    cout << "调用析构函数" << endl;
    struct epoll_event ev;
    // 超时的一定都是读请求，没有"被动"写。
    ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    ev.data.ptr = (void*)this;
    epoll_del(epoll_fd, fd, &ev);
    if (timer != NULL)
    {
        timer->clearReq();
        timer = NULL;
    }
    close(fd);
}

void ner_connect::setFd(int _fd)
{
    this->fd = _fd;
}

int ner_connect::getFd()
{
    return this->fd;
}

void ner_connect::seperateTimer()
{
    if (timer)
    {
        timer->clearReq();
        timer = NULL;
    }
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
            if (method == METHOD_POST)
            {
                state = STATE_RECV_BODY;
            }
            else
            {
                state = STATE_ANALYSIS;
            }
        }
        if (state == STATE_RECV_BODY)
        {
            //
        }

        if (state == STATE_ANALYSIS)
        {
            int flag = this->httpConnect();
            if(flag < 0){
                delete this;
                return ;
            }
        }
    }
    if (state == STATE_FINISH)
    {
        if (keep_alive)
        {
            printf("ok\n");
            this->reset();
        }
        else
        {
            delete this;
            return;
        }
    }

    pthread_mutex_lock(&qlock);
    nerTimer *mtimer = new nerTimer(this, 500);
    timer = mtimer;
    nerTimeQueue.push(mtimer);
    pthread_mutex_unlock(&qlock);
    struct epoll_event event;

    __uint32_t _epo_event = EPOLLIN | EPOLLET | EPOLLONESHOT;
    event.events = _epo_event;
    event.data.ptr = (void *)this;
    int ret = epoll_mod(epoll_fd, fd, &event);
    if (ret < 0)
    {
        // 返回错误处理
        delete this;
        return;
    }
    cout<<"connect is down"<<endl;
    //epoll_event _event;
    //_event.data.ptr = this;
    //_event.events = EPOLLIN | EPOLLET;
    //int ret = epoll_mod(epoll_fd, fd, &_event);
    //if (ret < 0)
    //{
    //    // 返回错误处理
    //    delete this;
    //    return;
    //}
}

void ner_connect::reset()
{
    content.clear();
    file_name.clear();
    state = STATE_PARSE_URI;
    headers.clear();
    keep_alive = false;
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
                file_name += uri_str.substr(pos + 1, _pos - pos - 1);
                int _pos2 = file_name.find('?');
                if (_pos2 >= 0)
                {
                    file_name = file_name.substr(0, _pos2);
                }
            }
            else
            {
                file_name += "index.html";
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
    //cout << "http version : " << http_ver << endl;
    state = STATE_PARSE_HEADERS;
    //cout << "content : " << content << endl;
    return PARSER_SUCCESS;
}

int ner_connect::parseHeader()
{
    string header = content;
    int pos = 0;
    for (int i = 0; i + 1 < header.size();)
    {
        if (header[i] == '\r' && header[i + 1] == '\n')
        {
            pos += 2;
            cout << "parse header end" << endl;
            break;
        }
        else if (header[i] == '\n')
        {
            pos = ++i;
            continue;
        }
        else
        {
            int pos1 = header.find(':', pos);
            if (pos < 0)
            {
                return PARSER_ERROR;
            }
            else
            {
                int pos2 = header.find('\r', pos);
                if (pos2 < 0)
                {
                    return PARSER_ERROR;
                }
                else
                {
                    string key = header.substr(pos, pos1 - pos);
                    string value = header.substr(pos1 + 2, pos2 - pos1 - 2);
                    headers[key] = value;
                    i = pos2 + 1;
                }
            }
            pos = i;
        }
    }
    content = header.substr(pos);
    //cout << content << endl;
    //cout << "header : **************************************" << endl;
    //for (auto temp : headers)
    //{
    //    cout << temp.first << "@:@ " << temp.second << endl;
    //}
    //cout << "header : **************************************" << endl;
    return PARSER_SUCCESS;
}

int ner_connect::httpConnect()
{
    if (method == METHOD_GET)
    {
        cout << "connect !!!" << endl;
        char header[MAXLINE];
        sprintf(header, "HTTP/1.1 %d %s\r\n", 200, "OK");
        if (headers.find("Connection") != headers.end() && headers["Connection"] == "keep-alive")
        {
            keep_alive = true;
            sprintf(header, "%sConnection: keep-alive\r\n", header);
            sprintf(header, "%sKeep-Alive: timeout=%d\r\n", header, WAIT_TIME);
        }

        int dot_pos = file_name.find('.');
        const char *filetype;
        std::string temp = "default";
        if (dot_pos < 0)
            filetype = ner_mime_type2value(temp).c_str();
        else
            filetype = ner_mime_type2value(file_name.substr(dot_pos)).c_str();
        struct stat sbuf;
        if (stat(file_name.c_str(), &sbuf) < 0)
        {
            httpError404(fd);
            return -1;
        }

        sprintf(header, "%sContent-type: %s\r\n", header, filetype);
        // 通过Content-length返回文件大小
        sprintf(header, "%sContent-length: %ld\r\n", header, sbuf.st_size);

        sprintf(header, "%s\r\n", header);
        size_t send_len = (size_t)writen(fd, header, strlen(header));
        if (send_len != strlen(header))
        {
            perror("Send header failed");
            return -1;
        }
        int src_fd = open(file_name.c_str(), 00, 0);
        char *src_addr = static_cast<char *>(mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0));
        close(src_fd);

        // 发送文件并校验完整性
        send_len = writen(fd, src_addr, sbuf.st_size);
        if (send_len != sbuf.st_size)
        {
            perror("Send file failed");
            return -1;
        }
        munmap(src_addr, sbuf.st_size);
        //cout<<"*******************************************"<<endl;
        //cout<<header;
        //cout<<"*******************************************"<<endl;
        return 0;
    }
}


void ner_connect::setTimer(nerTimer* mtimer){
    if(timer == NULL){
        timer = mtimer;
    }
}