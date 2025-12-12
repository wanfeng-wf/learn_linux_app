#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define PORT 8888
#define BUFFER_SIZE 1024

static int IS_SERVER_RUN = 1;

void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        IS_SERVER_RUN = 0;
    }
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

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
        exit(EXIT_FAILURE);
    }

    // 设置端口复用，防止 Ctrl+C 后端口被占用
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
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("[Server] Listening on port %d... Waiting for connection.\n", PORT);

    while (IS_SERVER_RUN)
    {
        // 阻塞等待连接
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

        printf("[Server] Connection accepted from %s, FD: %d\n", inet_ntoa(address.sin_addr), client_fd);

        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            int valread = read(client_fd, buffer, BUFFER_SIZE);
            if (valread <= 0)
            {
                printf("[Server] Client disconnected.\n");
                break;
            }

            printf("[Server] Received: %s", buffer);

            char msg[BUFFER_SIZE + 32];
            sprintf(msg, "Echo: %s", buffer);
            send(client_fd, msg, strlen(msg), 0);
        }
        close(client_fd);
        printf("[Server] Connection closed. Waiting for next connection...\n");
    }

    // 退出
    close(server_fd);
    printf("\n[Server] Server closed cleanly.\n");
    return 0;
}