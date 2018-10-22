#pragma once
#include <pthread.h>
#include <vector>

const int MAX_THREADS = 1024;
//最大线程池初始化大小以及任务队列大小
struct ner_task;

//自定义线程池返回状态,五种错误状态
enum
{
    THREADPOOL_INVALID = -1,
    THREADPOOL_LOCK_FAILURE = -2,
    THREADPOOL_QUEUE_FULL = -3,
    THREADPOOL_SHUTDOWN = -4,
    THREADPOOL_THREAD_FAILURE = -5,
    THREADPOOL_COND_BROADCAST = -6
} ner_threadpool_error;

//线程池状态参数
typedef enum
{
    immediate_shutdown = 1,
    graceful_shutdown = 2
} ner_threadpool_shutdown;

//任务结构体,包含函数指针以及参数列表
struct ner_task
{
    void (*func)(void *);
    void *arg;
    ner_task *next;
};

//定义线程池结构体
struct ner_threadpool
{
    pthread_mutex_t lock; //互斥锁
    pthread_cond_t cond;  //信号量
    pthread_t *threads;   //线程列表
    ner_task *task;       //任务队列
    int thread_count;     //线程数量
    int task_size;        //等待任务数量
    int shutdown;         //线程池状态
    int started;          //线程池开始位置

    
};

//线程池API

ner_threadpool *threadpool_create(int thread_count);
int threadpool_add(ner_threadpool *ner_pool, void (*func)(void *), void *argument);
int threadpool_destroy(ner_threadpool *ner_pool, int graceful);
int threadpool_free(ner_threadpool *pool);
void * threadpool_work(void *arg);