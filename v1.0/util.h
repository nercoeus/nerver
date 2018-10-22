#ifndef UTIL_H_
#define UTIL_H_

#include <cstdlib>

class nerMutexLock
{
public:
    explicit nerMutexLock();
    ~nerMutexLock();

private:
    static pthread_mutex_t lock;

private:
    nerMutexLock(const nerMutexLock&);
    nerMutexLock& operator=(const nerMutexLock&);
};


ssize_t readn(int fd, void *buf, size_t n);
ssize_t writen(int fd, void *buf, size_t n);

#endif