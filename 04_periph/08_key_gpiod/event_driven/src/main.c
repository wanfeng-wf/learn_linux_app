#include <stdio.h>
#include <unistd.h>
#include <gpiod.h>

#define CHIP_NAME   "gpiochip0"
#define LINE_OFFSET 13 // GPIO0_B5

int main()
{
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    struct gpiod_line_event event;

    chip = gpiod_chip_open_by_name(CHIP_NAME);
    line = gpiod_chip_get_line(chip, LINE_OFFSET);

    // 请求监测双边沿事件
    if (gpiod_line_request_both_edges_events(line, "button-listener") < 0)
    {
        perror("Request events failed");
        return -1;
    }

    printf("Waiting for events on %s line %d...\n", CHIP_NAME, LINE_OFFSET);

    int i = 1;
    while (1)
    {
        // 阻塞等待事件，无超时 (NULL)，CPU 占用 0%
        if (gpiod_line_event_wait(line, NULL) > 0)
        {
            // 读取具体事件
            gpiod_line_event_read(line, &event);

            if (event.event_type == GPIOD_LINE_EVENT_FALLING_EDGE)
            {
                usleep(20 * 1000);
                if (0 == gpiod_line_get_value(line))
                    printf(">>> [%d] Button Pressed\n", i++);
            }
            else
            {
                usleep(20 * 1000);
                if (1 == gpiod_line_get_value(line))
                    printf("    [%d] Button Released\n", i - 1);
            }
        }
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return 0;
}