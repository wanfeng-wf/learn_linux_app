#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#define KEY_EVENT_PATH "/dev/input/event3"

int main()
{
    int fd = open(KEY_EVENT_PATH, O_RDONLY);
    struct input_event ie;

    while (read(fd, &ie, sizeof(ie)) > 0)
    {
        // EV_KEY 代表按键事件
        if (ie.type == EV_KEY)
        {
            // value: 1=按下, 0=松开, 2=长按连发
            printf("Key Code: %d, Action: %s\n",
                   ie.code,
                   ie.value == 1 ? "Pressed" : (ie.value == 0 ? "Released" : "Repeat"));
        }
    }
    return 0;
}