#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#define PORT 8888
#define BUFFER_SIZE 1024

static int IS_SERVER_RUN = 1;

void signal_handler(int signum);
// 子线程函数：专门负责和一个客户端聊天
void *client_handler(void *arg);

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;

    // sigaction 可以更精细地控制信号行为，特别是禁止自动重启系统调用
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // 关键：不设置 SA_RESTART，让 accept 被信号打断后返回 -1
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(1);
    }

    // 端口复用
#ifdef SO_REUSEPORT
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(1);
    }
#else
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(1);
    }
#endif

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(1);
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        exit(1);
    }

    printf("[Main] Multi-threaded concurrent server started! Listening on port %d\n", PORT);

    // --- 主循环：只负责 Accept 和 Create Thread ---
    while (IS_SERVER_RUN)
    {
        // 1. 阻塞等待新连接
        client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        // 处理 accept 的返回值
        if (client_fd < 0)
        {
            // 如果是因为被信号中断 (Ctrl+C) 而失败
            if (errno == EINTR)
            {
                // 此时 IS_SERVER_RUN 已经被 handler 改为 0 了
                // continue 会回到 while 判断，然后退出循环
                continue;
            }
            // 如果是其他真正的错误
            perror("accept");
            break;
        }

        printf("[Main] New connection accepted! IP: %s, Assigned FD: %d\n",
               inet_ntoa(address.sin_addr), client_fd);

        // 2. 创建新线程处理该连接
        pthread_t tid;
        // 技巧：直接把 client_fd 当作参数值传进去 (cast to void*)
        // 只要 int 的大小不大于 void* (在64位系统肯定没问题)，这就安全
        if (pthread_create(&tid, NULL, client_handler, (void *)(long)client_fd) != 0)
        {
            perror("pthread_create");
            close(client_fd); // 创建线程失败，必须关闭连接，否则资源泄漏
        }

        // 主线程立刻回头去 accept，不在这里 wait/join
    }

    close(server_fd);
    printf("\n[Main] Server closed.\n");
    return 0;
}

void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        IS_SERVER_RUN = 0;
    }
}

void *client_handler(void *arg)
{
    // 1. 获取主线程传过来的 client_fd
    // 注意：这里用的是 "值传递" 的技巧 (long)arg
    // 也可以用 malloc 分配内存传指针，但在 fd 这种简单整数上，直接传值更方便且无内存泄漏风险
    int client_fd = (int)(long)arg;

    char buffer[BUFFER_SIZE];
    int valread;

    // 2. 分离线程 (Detach)
    // 设置分离属性。
    // 意味着主线程不需要 join 它，它结束后系统自动回收资源。
    // 这对于服务器非常重要，因为主线程没空去一个个 join 子线程。
    pthread_detach(pthread_self());

    printf("[Thread-%lu] Started serving client FD: %d\n", (unsigned long)pthread_self(), client_fd);

    // 3. 循环通信 (Echo)
    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        valread = read(client_fd, buffer, BUFFER_SIZE);

        if (valread <= 0)
        {
            // 客户端断开或出错
            printf("[Thread-%lu] Client (FD:%d) disconnected.\n", (unsigned long)pthread_self(), client_fd);
            break;
        }

        // 打印收到什么 (去掉末尾可能的换行符方便显示)
        buffer[strcspn(buffer, "\n")] = 0;
        printf("[Thread-%lu] Received (FD:%d): %s\n", (unsigned long)pthread_self(), client_fd, buffer);

        // 回显
        char msg[BUFFER_SIZE + 32];
        sprintf(msg, "Server Echo: %s\n", buffer);
        write(client_fd, msg, strlen(msg));
    }

    // 4. 收尾
    close(client_fd); // 关掉分机
    return NULL;      // 线程结束，自动释放栈空间
}