#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "i2c_lib.h"
#include <errno.h>

int main()
{
    int bus = 2;
    int fd = i2c_init(bus);
    if (fd < 0)
    {
        perror("Init I2C device failed");
        return -1;
    }

    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");

    // 7位地址有效的设备地址范围为0x08到0x77
    for (int addr = 0; addr < 128; addr++)
    {
        // 打印行号
        if (addr % 16 == 0)
        {
            printf("%02x: ", addr);
        }

        // 跳过保留地址范围
        if (addr < 0x08 || addr > 0x77)
        {
            printf("   ");
            continue;
        }

        // 发送探测

        if (i2c_write(fd, addr, NULL, 0) < 0)
        {
            printf("-- ");         // 无设备 (NACK)
        }
        else
        {
            printf("%02x ", addr); // 发现设备 (ACK)
        }

        if (addr % 16 == 15)
        {
            printf("\n");
        }
    }
    printf("\n");

    i2c_deinit(fd);
    return 0;
}