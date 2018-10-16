#ifndef UTIL_H_
#define UTIL_H_

#include <cstdlib>

ssize_t readn(int fd, void* buf, size_t n);
ssize_t writen(int fd, void* buf, size_t n);


#endif