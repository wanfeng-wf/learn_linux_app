#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH "/sys/class/gpio/unexport"
#define GPIO_DIR_PATH "/sys/class/gpio/gpio%d/direction"
#define GPIO_VAL_PATH "/sys/class/gpio/gpio%d/value"

int gpio_init(int pin, const char *direction)
{
    // 导出 GPIO 引脚
    FILE *export_file = fopen(GPIO_EXPORT_PATH, "w");
    if (export_file == NULL)
    {
        return -1;
    }
    fprintf(export_file, "%d", pin);
    fclose(export_file);

    // 设置 GPIO 方向
    char direction_path[64];
    snprintf(direction_path, sizeof(direction_path), GPIO_DIR_PATH, pin);
    FILE *direction_file = fopen(direction_path, "w");
    if (direction_file == NULL)
    {
        return -1;
    }
    fprintf(direction_file, "%s", direction);
    fclose(direction_file);
    return 0;
}

int gpio_deinit(int pin)
{
    FILE *unexport_file = fopen(GPIO_UNEXPORT_PATH, "w");
    if (unexport_file == NULL)
    {
        return -1;
    }
    fprintf(unexport_file, "%d", pin);
    fclose(unexport_file);
    return 0;
}

int gpio_set_value(int pin, int value)
{
    char value_path[64];
    snprintf(value_path, sizeof(value_path), GPIO_VAL_PATH, pin);
    FILE *value_file = fopen(value_path, "w");
    if (value_file == NULL)
    {
        return -1;
    }
    fprintf(value_file, "%d", value);
    fclose(value_file);
    return 0;
}

int gpio_get_value(int pin)
{
    char value_path[64];
    snprintf(value_path, sizeof(value_path), GPIO_VAL_PATH, pin);
    FILE *value_file = fopen(value_path, "r");
    if (value_file == NULL)
    {
        return -1;
    }
    char value = '0';
    if (fscanf(value_file, "%c", &value) != 1)
    {
        fclose(value_file);
        return -1;
    }
    fclose(value_file);
    return value - '0';
}