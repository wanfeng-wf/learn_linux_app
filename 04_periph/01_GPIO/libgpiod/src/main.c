#include <stdio.h>
#include <unistd.h>
#include <gpiod.h>

int main(int argc, char **argv)
{
    unsigned int chip_num;
    printf("Please enter the GPIO chip number: ");
    scanf("%u", &chip_num);

    unsigned int line_num;
    printf("Please enter the GPIO line number: ");
    scanf("%u", &line_num);

    struct gpiod_chip *chip;
    struct gpiod_line *line;

    // 1. 打开 GPIO 控制器
    chip = gpiod_chip_open_by_number(chip_num);
    if (!chip)
    {
        perror("Open chip failed");
        return -1;
    }

    // 2. 获取对应的引脚 (Line)
    line = gpiod_chip_get_line(chip, line_num);
    if (!line)
    {
        perror("Get line failed");
        gpiod_chip_close(chip);
        return -1;
    }

    // 3. 申请引脚为输出模式，初始值 0，消费名为 "my_gpio"
    // 这一步相当于 Sysfs 的 export + direction + value
    if (gpiod_line_request_output(line, "my_gpio", 0) < 0)
    {
        perror("Request line as output failed");
        gpiod_chip_close(chip);
        return -1;
    }

    int value = 0;
    int ret = 0;
    // 4. 循环控制 GPIO 引脚电平
    for (int i = 0; i < 6; i++)
    {
        value = !value;
        gpiod_line_set_value(line, value);
        ret = gpiod_line_get_value(line);
        printf("%d\n", ret);
        sleep(1);
    }

    // 5. 释放资源
    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}
