#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
int main(int argc, char **argv)
{
    printf("begin\n");
    int port, clientfd;
    struct hostent *hp;
    char *EXIT = "exit is getted";
    struct sockaddr_in serveraddr;
    port = atoi(argv[2]);
    printf("begin 2\n");
    if ((hp = gethostbyname(argv[1])) == NULL)
    {
        printf("ip error\n");
    }
    printf("begin 3\n");
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error\n");
    }
    bcopy((char *)hp->h_addr_list[0], (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    if (connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
        printf("connect error\n");
    }
    else
    {
        printf("connect successful!\n");
        while (1)
        {
            char buf[1024];
            char get[1024];
            fprintf(stderr, "please write data %s\n", buf);
            fgets(buf, sizeof(buf), stdin);
            write(clientfd, buf, sizeof(buf));
            printf("%s, %lu\n", buf, strlen(buf));
            fprintf(stderr, "数据写入完毕 %s\n", buf);
            //read(clientfd, get, sizeof(get));
            //fprintf(stderr, "read data: data %s\n", get);
            bzero(buf, 1024);
        }
    }
    return 0;
}
