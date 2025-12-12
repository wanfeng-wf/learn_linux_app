#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

sem_t g_sem;

void *producer(void *arg)
{
    int val;
    for (int i = 1; i <= 5; i++)
    {
        sleep(1);
        sem_post(&g_sem); // 生产一个

        // === 查看当前库存 ===
        sem_getvalue(&g_sem, &val);
        printf("[Producer] Made #%d. Current Sem Value: %d\n", i, val);
    }
    return NULL;
}

void *consumer(void *arg)
{
    int val;
    for (int i = 1; i <= 5; i++)
    {
        sem_wait(&g_sem); // 消费一个

        // === 查看当前库存 ===
        sem_getvalue(&g_sem, &val);
        printf("  [Consumer] Ate #%d. Current Sem Value: %d\n", i, val);

        sleep(2); // 消费者吃得慢一点，积压一下看看
    }
    return NULL;
}

int main()
{
    pthread_t t1, t2;
    // 初始化为 0
    sem_init(&g_sem, 0, 0);

    pthread_create(&t1, NULL, producer, NULL);
    pthread_create(&t2, NULL, consumer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    sem_destroy(&g_sem);
    return 0;
}