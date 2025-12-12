#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define UART_PATH "/dev/ttyS%d"

int uart_init(int port, int speed)
{
    // 1. 打开串口
    // O_RDWR: 读写权限
    // O_NOCTTY: 该端口不作为控制终端(防止 Ctrl+C 杀掉程序)
    // O_SYNC: 同步模式, 写操作会阻塞直到数据被写入
    char path[20];
    snprintf(path, sizeof(path), UART_PATH, port);
    int fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        perror("Error opening serial port");
        return -1;
    }

   // 2. 配置串口参数
    struct termios tty;

    // 1. 获取当前参数
    if (tcgetattr(fd, &tty) < 0)
    {
        perror("Error from tcgetattr");
        return -1;
    }

    // 2. 设置输入输出波特率
    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    // 3. 设置字符大小、无校验、1个停止位 (8N1)
    tty.c_cflag &= ~PARENB; // 无校验
    tty.c_cflag &= ~CSTOPB; // 1个停止位
    tty.c_cflag &= ~CSIZE;  // 清除数据位掩码
    tty.c_cflag |= CS8;     // 8个数据位

    // 4. 禁用硬件流控 (RTS/CTS)
    tty.c_cflag &= ~CRTSCTS;

    // 5. 启用接收器，本地连接
    tty.c_cflag |= CREAD | CLOCAL;

    // 6. 设置原始模式 (Raw Mode) - 关键！
    // 默认是规范模式（行缓冲），我们需要原始模式处理二进制数据
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // 禁用软件流控, 输出流控(IXON)、输入流控(IXOFF) 和 任意字符重启(IXANY)
    tty.c_oflag &= ~OPOST;                  // 禁用输出处理（如换行转换）

    // 7. 设置阻塞读取行为
    // VMIN=0, VTIME=10: 读不到数据时等待 1秒 (10*0.1s) 后返回
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;

    // 8. 应用配置
    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        perror("Error from tcsetattr");
        return -1;
    }

    return fd;
}

void uart_deinit(int fd)
{
    close(fd);
}

int uart_send(int fd, char *data, int len)
{
    int wlen = write(fd, data, len);

    if (wlen < 0)
    {
        return -1;
    }
    else
    {
        return wlen;
    }
}

int uart_receive(int fd, char *buf, int len)
{
    int rlen = read(fd, buf, len);
    
    if (rlen < 0)
    {
        return -1;
    }
    else
    {
        return rlen;
    }
}