#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <time.h> 
#include <errno.h>

int main(int argc, char **argv)
{
    int fd;
    struct input_event ev;
    struct pollfd fds[1]; // 定义 poll 结构体数组（虽然这里只监听 1 个）
    int ret;

    if (argc != 2)
    {
        printf("Usage: %s <device_path>\n", argv[0]);
        printf("Example: %s /dev/input/event0\n", argv[0]);
        return -1;
    }

    // 1. 打开设备
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("open device");
        return -1;
    }

    // 2. 配置 poll 监听结构体
    fds[0].fd = fd;         // 监听哪个文件？
    fds[0].events = POLLIN; // 监听什么事件？ POLLIN 表示“有数据可读”

    printf("Listening on %s (Timeout: 5s)...\n", argv[1]);

    while (1)
    {
        // 3. 调用 poll (核心)
        // 参数: 结构体数组, 数组大小, 超时时间(毫秒)
        // 5000ms = 5秒
        ret = poll(fds, 1, 5000);

        if (ret == 0)
        {
            // === 情况 A: 超时 (5秒内没人动鼠标) ===
            printf("Timeout! (I am still alive, can do other jobs...)\n");
        }
        else if (ret < 0)
        {
            // === 情况 B: 出错 ===
            perror("poll error");
            break;
        }
        else
        {
            // === 情况 C: 有事件发生 (ret > 0) ===
            // 判断是否是我们监听的那个文件发生了 POLLIN 事件
            if (fds[0].revents & POLLIN)
            {
                // 此时再调用 read 不会阻塞，因为 poll 保证了有数据可读
                int len = read(fd, &ev, sizeof(ev));

                if (len == sizeof(ev))
                {
                    // 时间格式化
                    struct tm *tm_info;
                    char time_fmt[64];
                    time_t raw_time = ev.time.tv_sec;
                    tm_info = localtime(&raw_time);
                    strftime(time_fmt, sizeof(time_fmt), "%H:%M:%S", tm_info);

                    // 打印关键信息
                    printf("[%s] Type: %d, Code: %d, Value: %d\n",
                           time_fmt, ev.type, ev.code, ev.value);
                }
            }
        }
    }

    close(fd);
    return 0;
}