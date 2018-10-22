#include <stdio.h>
#include <malloc.h>
#include "threadpool.h"

ner_threadpool * threadpool_create(int thread_count)
{
    if (thread_count <= 0 || thread_count > MAX_THREADS)
    {
        perror("this ");
        return NULL;
    }
    ner_threadpool *pool = (ner_threadpool *)malloc(sizeof(ner_threadpool));
    if (pool == NULL)
    {
        perror("this 2 ");
        goto err;
    }
    pool->thread_count = 0;
    pool->started = 0;
    pool->task_size = 0;
    pool->shutdown = 0;
    pool->task = (ner_task *)malloc(sizeof(ner_task));
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);

    if (pool->task == NULL || pool->threads == NULL)
    {
        perror("this 3 ");
        goto err;
    }

    pool->task->next = NULL;
    pool->task->arg = NULL;
    pool->task->func = NULL;

    if (pthread_mutex_init(&pool->lock, NULL) != 0)
    {
        perror("this 4 ");
        goto err;
    }
    if (pthread_cond_init(&pool->cond, NULL) != 0)
    {
        perror("this 5 ");
        pthread_mutex_unlock(&pool->lock);
        goto err;
    }

    int i;
    for (i = 0; i < thread_count; i++)
    {
        if (pthread_create(&(pool->threads[i]), NULL, threadpool_work, (void *)pool) != 0)
        {
            threadpool_destroy(pool, 0);
            return NULL;
        }
        pool->thread_count++;
        pool->started++;
    }

    return pool;
err:
    if (pool)
    {
        threadpool_free(pool);
    }
    return NULL;
}

int threadpool_add(ner_threadpool *pool, void (*func)(void *), void *arg)
{
    int rc, er = 0;
    if (pool == NULL || func == NULL)
    {
        return -1;
    }
    if (pthread_mutex_lock(&(pool->lock)) != 0)
    {
        return -1;
    }
    do
    {
        if (pool->shutdown)
        {
            er = THREADPOOL_SHUTDOWN;
            break;
        }
        ner_task *task = (ner_task *)malloc(sizeof(ner_task));
        if (task == NULL)
        {
            break;
        }
        task->func = func;
        task->arg = arg;
        task->next = pool->task->next;
        pool->task->next = task;
        pool->task_size++;
        rc = pthread_cond_signal(&pool->cond);
    } while (0);

    if (pthread_mutex_unlock(&pool->lock) != 0)
    {
        return -1;
    }
    return er;
}

int threadpool_destroy(ner_threadpool *pool, int graceful)
{
    int err = 0;
    if (pool == NULL)
    {
        fprintf(stderr, "pool is null\n");
        return THREADPOOL_INVALID;
    }
    if (pthread_mutex_lock(&(pool->lock)) != 0)
    {
        fprintf(stderr, "mutex is lock\n");
        return THREADPOOL_LOCK_FAILURE;
    }
    do
    {
        if (pool->shutdown)
        {
            err = THREADPOOL_SHUTDOWN;
            break;
        }
        pool->shutdown = (graceful) ? graceful_shutdown : immediate_shutdown;

        if (pthread_cond_broadcast(&(pool->cond)) != 0)
        {
            err = THREADPOOL_COND_BROADCAST;
            break;
        }
        if (pthread_mutex_unlock(&(pool->lock)) != 0)
        {
            err = THREADPOOL_LOCK_FAILURE;
            break;
        }
        int i = 0;
        for (; i < pool->thread_count; i++)
        {
            if (pthread_join(pool->threads[i], NULL) != 0)
            {
                err = THREADPOOL_THREAD_FAILURE;
            }
        }
    } while (0);
    if (err)
    {
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->cond));
        threadpool_free(pool);
    }
    return err;
}

int threadpool_free(ner_threadpool *pool)
{
    if (pool == NULL || pool->started > 0)
    {
        return -1;
    }
    if (pool->threads)
    {
        free(pool->threads);
    }
    ner_task *old;
    while (pool->task->next)
    {
        old = pool->task->next;
        pool->task->next = old->next;
        free(old);
    }
    return 0;
}

//传入ner_threadpool*类型的参数
void * threadpool_work(void *arg)
{
    printf("%lu\n", pthread_self());
    if (arg == NULL)
    {
        return NULL;
    }
    ner_threadpool *pool = (ner_threadpool *)arg;
    ner_task *task;

    while (1)
    {
        pthread_mutex_lock(&(pool->lock));
        while ((pool->task_size == 0) && !(pool->shutdown))
        {
            pthread_cond_wait(&(pool->cond), &(pool->lock));
        }

        if (pool->shutdown == immediate_shutdown)
        {
            break;
        }
        else if (pool->shutdown == graceful_shutdown && pool->task_size == 0)
        {
            break;
        }

        task = pool->task->next;
        if (task == NULL)
        {
            pthread_mutex_unlock(&pool->lock);
            continue;
        }
        pool->task->next = task->next;
        pool->task_size--;

        pthread_mutex_unlock(&pool->lock);

        (*(task->func))(task->arg);
        printf("thread is end\n");

        free(task);
    }
    pool->started--;
    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return NULL;
}
