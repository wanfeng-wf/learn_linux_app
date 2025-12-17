#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 8888
#define SERVER_IP   "192.168.7.2" // 板子IP

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    // 1. 创建 UDP Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    // 2. 配置目标地址
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(SERVER_PORT);
    inet_pton(AF_INET, (argc > 1 ? argv[1] : SERVER_IP), &serv_addr.sin_addr);

    while (1)
    {
        printf("Input > ");
        fgets(buffer, sizeof(buffer), stdin);

        // 3. 发送数据 (指定发给 serv_addr)
        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&serv_addr, sizeof(serv_addr));

        // 4. 接收回复
        memset(buffer, 0, sizeof(buffer));
        int len = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                           NULL, NULL); // 不关心是谁回的，填NULL
        if (len > 0)
        {
            buffer[len] = '\0';
            printf("Echo: %s\n", buffer);
        }
    }
    close(sockfd);
    return 0;
}