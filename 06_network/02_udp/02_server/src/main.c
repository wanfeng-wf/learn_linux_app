#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT        8888
#define BUFFER_SIZE 1024

int main()
{
    int sockfd;
    struct sockaddr_in serv_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // 1. 创建 UDP Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    // 2. 绑定地址 (Server 必须 Bind)
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // 监听所有网卡
    serv_addr.sin_port        = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    printf("[UDP Server] Listening on port %d...\n", PORT);

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        // 3. 接收数据 (同时获取对方地址)
        int len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                           (struct sockaddr *)&client_addr, &addr_len);

        if (len > 0)
        {
            buffer[len] = '\0'; // 添加字符串结束符
            printf("Received from %s: %s\n",
                   inet_ntoa(client_addr.sin_addr), buffer);

            // 4. 回复数据 (发回给 sender)
            sendto(sockfd, buffer, len, 0,
                   (struct sockaddr *)&client_addr, addr_len);
        }
    }
    close(sockfd);
    return 0;
}