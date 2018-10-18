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