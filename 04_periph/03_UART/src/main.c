#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include "uart.h"

int main(int argc, char **argv)
{
    int port = 3; // 默认串口，可根据参数修改
    if (argc > 1)
        port = atoi(argv[1]);

    printf("Opening /dev/ttyS%d...\n", port);

    int fd = uart_init(port, B115200);
    if (fd < 0)
    {
        return -1;
    }

    // 3. 发送数据
    char *msg = "Hello World!\n";
    int len = strlen(msg);;
    if (uart_send(fd, msg, len) != len)
    {
        return -1;
    }
    printf("Sent: %s", msg);

    // 4. 接收数据
    // 稍微延时，确保数据发出去并（如果是回环）传回来
    usleep(100000);

    char buf[256];
    // 清空 buffer
    memset(buf, 0, sizeof(buf));

    if (uart_receive(fd, buf, sizeof(buf) - 1) != len)
    {
        return -1;
    }
    printf("Received: %s", buf);

    // 5. 关闭
    uart_deinit(fd);

    return 0;
}