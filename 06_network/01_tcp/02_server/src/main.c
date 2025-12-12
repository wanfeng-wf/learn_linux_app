#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address; // IPv4 地址结构体
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    printf("[Server] Creating socket...\n");
    // 1. 创建 Socket
    // AF_INET: IPv4
    // SOCK_STREAM: TCP (如果是 UDP 则用 SOCK_DGRAM)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. 绑定 (Bind) IP 和端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 监听本机所有网卡 (Wi-Fi, 网口)
    address.sin_port = htons(PORT);       // 注意：端口号必须转为网络字节序(Big Endian)

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. 监听 (Listen)
    // 3: 待处理连接队列的最大长度
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("[Server] Listening on port %d... Waiting for connection.\n", PORT);

    // 4. 接受连接 (Accept) - 会阻塞在这里，直到有客户端连上来
    // accept 返回一个新的 fd (new_socket) 专门用于和这个客户端通信
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // 打印客户端的 IP
    printf("[Server] Connection accepted from %s\n", inet_ntoa(address.sin_addr));

    // 5. 循环通信
    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        // 读取数据
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread <= 0)
        {
            // read 返回 0 表示客户端断开了连接
            printf("[Server] Client disconnected.\n");
            break;
        }

        printf("[Server] Received: %.*s", valread, buffer);

        // 回显数据 (Echo)
        char msg[BUFFER_SIZE + 32];
        sprintf(msg, "Echo: %s", buffer);
        send(new_socket, msg, strlen(msg), 0);
    }

    // 6. 关闭连接
    close(new_socket); // 关闭与客户端的连接
    close(server_fd);  // 关闭服务器监听
    printf("[Server] Server closed.\n");
    return 0;
}