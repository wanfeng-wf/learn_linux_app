#include <stdio.h>
#include <unistd.h>
#include <gpiod.h>

#define CHIP_NAME   "gpiochip0"
#define LINE_OFFSET 13 // GPIO0_B5
#define DEBOUNCE_MS 20 // 消抖时间


int main()
{
    struct gpiod_chip *chip = gpiod_chip_open_by_name(CHIP_NAME);
    struct gpiod_line *line = gpiod_chip_get_line(chip, LINE_OFFSET);
    int i, val, last_val = 1; // 假设初始状态为高（松开）

    // 仅配置为输入模式 (不使用中断)
    if (gpiod_line_request_input(line, "debounce-worker") < 0)
    {
        perror("Request input failed");
        return -1;
    }

    i = 1;
    while (1)
    {
        // 轮询当前值
        val = gpiod_line_get_value(line);

        // 发现电平变化
        if (val != last_val)
        {
            // 消抖 20ms
            usleep(DEBOUNCE_MS * 1000);

            // 再次读取进行确认
            int stable_val = gpiod_line_get_value(line);

            if (stable_val == val)
            {
                if (stable_val == 0)
                    printf(">>> [%d] Button Pressed\n", i++);
                else
                    printf("    [%d] Button Released\n", i-1);

                last_val = stable_val;
            }
        }

        // 降低轮询频率，保护 CPU 
        usleep(10 * 1000);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return 0;
}