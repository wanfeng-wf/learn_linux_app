#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// 这是子线程要执行的代码
void *worker_thread(void *arg)
{
    // 获取当前线程 ID
    pthread_t tid = pthread_self();

    printf("  [Worker] I am a new thread! My ID: %lu\n", (unsigned long)tid);

    for (int i = 0; i < 3; i++)
    {
        printf("  [Worker] Working... %d\n", i);
        sleep(1);
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_t tid; // 用于存放子线程的 ID
    int ret;

    printf("[Main] I am the main thread. My ID: %lu\n", (unsigned long)pthread_self());

    // 1. 创建线程
    printf("[Main] Creating a worker thread...\n");
    ret = pthread_create(&tid, NULL, worker_thread, NULL);
    if (ret != 0)
    {
        fprintf(stderr, "pthread_create error: %d\n", ret);
        return 1;
    }

    printf("[Main] Worker thread created with ID: %lu\n", (unsigned long)tid);

    // 2. 模拟主线程也在做其他事
    for (int i = 0; i < 3; i++)
    {
        printf("[Main] I am managing... %d\n", i);
        sleep(1);
    }

    // 3. 等待子线程结束
    // 如果不加这行，主线程 return 退出后，子线程会被强制杀死
    printf("[Main] Waiting for worker to finish...\n");
    ret = pthread_join(tid, NULL);
    if (ret != 0)
    {
        printf("[Main] Worker not finish.\n");
        exit(ret);
    }

    printf("[Main] Worker finished. Exiting.\n");
    return 0;
}