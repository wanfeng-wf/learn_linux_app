#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gpio.h"

int main()
{
    int gpio_pin = 0;
    int value = 0;
    int ret = 0;

    printf("Please enter the GPIO pin number: ");
    scanf("%d", &gpio_pin);

    // 初始化 GPIO 引脚
    if (gpio_init(gpio_pin, "out") < 0)
    {
        printf("Failed to initialize GPIO pin %d\n", gpio_pin);
        return -1;
    }

    // 控制 GPIO 引脚电平
    for (int i = 0; i < 6; i++)
    {
        value = !value;
        gpio_set_value(gpio_pin, value);
        ret = gpio_get_value(gpio_pin);
        if (ret < 0)
        {
            gpio_deinit(gpio_pin);
            return -1;
        }
        printf("%d\n", ret);
        sleep(1);
    }

    // 取消GPIO引脚
    gpio_deinit(gpio_pin);

    return 0;
}
