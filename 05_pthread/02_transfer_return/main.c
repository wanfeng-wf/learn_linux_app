#include <pthread.h>
#include <stdio.h>

typedef struct
{
    int a;
    int b;
} Args;

void *worker(void *arg)
{
    Args *p = (Args *)arg; // 拿到主线程栈上的地址
    printf("Worker: %d + %d\n", p->a, p->b);
    return NULL;
}

int main()
{
    pthread_t tid;
    // 局部变量：在主线程的栈上
    Args my_args = {10, 20};

    // 传地址
    pthread_create(&tid, NULL, worker, (void *)&my_args);

    // 关键！必须在这里 join
    pthread_join(tid, NULL);

    return 0;
}