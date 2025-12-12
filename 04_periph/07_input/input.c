#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h> // 定义了 input_event 和 EV_KEY 等宏
#include <time.h>

int main(int argc, char **argv)
{
    int fd;
    struct input_event ev;

    // 1. 检查参数
    if (argc != 2)
    {
        printf("Usage: %s <device_path>\n", argv[0]);
        printf("Example: %s /dev/input/event0\n", argv[0]);
        return -1;
    }

    // 2. 打开设备节点 (阻塞模式)
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("open device");
        return -1;
    }

    printf("Reading input events from %s...\n", argv[1]);

    // 3. 循环读取事件
    while (1)
    {
        // read 会阻塞，直到有事件发生（按下按键、移动鼠标）
        int len = read(fd, &ev, sizeof(ev));

        if (len == sizeof(ev))
        {
            struct tm *tm_info;
            char time_fmt[64];
            time_t raw_time = ev.time.tv_sec; // 获取秒数

            tm_info = localtime(&raw_time); // 转为本地时间结构体
            strftime(time_fmt, sizeof(time_fmt), "%H:%M:%S", tm_info); // 格式化为字符串
            printf("[%s.%03ld] ", time_fmt, ev.time.tv_usec/1000);

            // 解析事件类型
            switch (ev.type)
            {
            case EV_SYN: // 同步事件 (0x00)，它像一个句号，用于分隔不同时刻的事件
                printf("Type: EV_SYN (Sync)\n");
                break;

            case EV_KEY: // 按键事件 (0x01)
                printf("Type: EV_KEY, Code: %d, Value: %d (%s)\n",
                       ev.code, ev.value,
                       ev.value == 1 ? "Press" : (ev.value == 0 ? "Release" : "Repeat"));
                break;

            case EV_REL: // 相对位移 (0x02) - 鼠标
                printf("Type: EV_REL, Code: %d (%s), Value: %d\n",
                       ev.code,
                       ev.code == REL_X ? "X" : (ev.code == REL_Y ? "Y" : "Other"),
                       ev.value);
                break;

            case EV_ABS: // 绝对位移 (0x03) - 触摸屏
                printf("Type: EV_ABS, Code: %d, Value: %d\n", ev.code, ev.value);
                break;

            default:
                printf("Type: 0x%x, Code: %d, Value: %d\n", ev.type, ev.code, ev.value);
            }
        }
    }

    close(fd);
    return 0;
}