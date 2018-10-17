#include "util.h"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

ssize_t readn(int fd, void *buf, size_t n)
{
    printf("fd : %d, n : %lu\n", fd, n);
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t readsum = 0;
    char *ptr = (char *)buf;

    while(nleft > 0){
        if((nread = read(fd, ptr, nleft)) < 0){
            if(errno == EAGAIN){
                return readsum;
            }else if(errno == EINTR){
                nread = 0;
            }
            else{
                return -1;
            }
        }
        else if(nread == 0){
            break;
        }
        nread = strlen(ptr);
        readsum += nread;
        ptr += nread;
        nleft -= nread;
        printf("readsum : %lu, n : %lu, buf:%s\n", readsum, nread, buf);
    }
    return readsum;
}

ssize_t writen(int fd, void *buf, size_t n)
{
    size_t nleft = n;
    ssize_t nwrite = 0;
    ssize_t writesum = 0;
    char *ptr = (char *)buf;

    while(nleft > 0){
        if((nwrite = write(fd, buf, nleft)) <= 0){
            if(nwrite < 0){
                if(errno == EINTR || errno == EAGAIN){
                    nwrite = 0;
                    continue;
                }
                else{
                    return -1;
                }
            }
        }
        writesum += nwrite;
        ptr += nwrite;
        nleft -= nwrite;
    }
    return writesum;
}

