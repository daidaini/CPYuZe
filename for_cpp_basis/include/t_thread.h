#pragma once

#include <pthread.h>
#include <unistd.h>

inline namespace dead_lock_test
{
    pthread_mutex_t mt_a = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mt_b = PTHREAD_MUTEX_INITIALIZER;

    void* thread_proc_A(void*)
    {
        printf("thread A waiting get Resource A \n");
        pthread_mutex_lock(&mt_a);
        printf("thread A Got Resource A\n");

        ::sleep(1);

        printf("thread A waiting get Resource B\n");
        pthread_mutex_lock(&mt_b);
        printf("thread A Got Resource B\n");

        pthread_mutex_unlock(&mt_b);
        pthread_mutex_unlock(&mt_a);
    }

    void* thread_proc_B(void*)
    {
        printf("thread B waiting get Resource B\n");
        pthread_mutex_lock(&mt_b);
        printf("thread B Got Resources B\n");

        ::sleep(1);

        printf("thread B waiting get Resource A\n");
        pthread_mutex_lock(&mt_a);
        printf("thread B Got Resource A\n");

        pthread_mutex_unlock(&mt_a);
        pthread_mutex_unlock(&mt_b);
    }

    void mock_dead_lock()
    {
        pthread_t tid_a, tid_b;

        //创建两个线程
        pthread_create(&tid_a, NULL, thread_proc_A, NULL);
        pthread_create(&tid_b, NULL, thread_proc_B, NULL);

        pthread_join(tid_a, NULL);
        pthread_join(tid_b, NULL);

        printf("mock_dead_lock end\n");
    }

}

