#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8888
#define BUFFER_SIZE 1024

static int IS_CLIENT_RUN = 1;

void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        IS_CLIENT_RUN = 0;
    }
}

int main(int argc, char const *argv[])
{
    int client_fd = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if (argc != 2)
    {
        printf("Usage: %s <Server_IP>\n", argv[0]);
        return -1;
    }

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // 不设置 SA_RESTART，让系统调用被信号打断后直接返回失败
    sigaction(SIGINT, &sa, NULL);

    // 1. 创建 Socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 2. 将 IP 字符串 (如 "192.168.1.10") 转为二进制网络格式
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address / Address not supported \n");
        return -1;
    }

    printf("[Client] Connecting to %s:%d...\n", argv[1], PORT);

    // 3. 连接服务器 (Connect)
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("[Client] Connected! Type text to send.\n");

    // 4. 发送数据
    while (IS_CLIENT_RUN)
    {
        printf("> ");
        fflush(stdout); // 确保提示符打印出来
        memset(buffer, 0, BUFFER_SIZE);

        // 如果 fgets 返回 NULL，可能是被信号打断了
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
        {
            if (IS_CLIENT_RUN == 0)
            {
                printf("\nInterrupted by user.\n");
                break; // 直接跳出循环
            }
        }

        // 发送
        send(client_fd, buffer, strlen(buffer), 0);

        // 接收回显
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(client_fd, buffer, BUFFER_SIZE);
        if (valread > 0)
        {
            printf("[Server Reply]: %.*s\n", valread, buffer);
        }
        else
        {
            printf("Server disconnected.\n");
            break;
        }
    }

    close(client_fd);
    printf("[Client] Connection closed.\n");
    return 0;
}