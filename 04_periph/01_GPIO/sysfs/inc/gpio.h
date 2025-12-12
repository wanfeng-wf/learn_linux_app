#ifndef _GPIO_H
#define _GPIO_H

/**
 * @brief 初始化 GPIO 引脚
 * @param pin GPIO 引脚号
 * @param direction GPIO 引脚方向，"in" 或 "out"
 * @return int 初始化结果，0 表示成功，-1 表示失败
 */
int gpio_init(int pin, const char *direction);

/**
 * @brief 释放 GPIO 引脚
 * @param pin GPIO 引脚号
 * @return int 释放结果，0 表示成功，-1 表示失败
 */
int gpio_deinit(int pin);

/**
 * @brief 设置 GPIO 引脚值
 * @param pin GPIO 引脚号
 * @param value GPIO 引脚值，0 或 1
 * @return int 设置结果，0 表示成功，-1 表示失败
 */
int gpio_set_value(int pin, int value);

/**
 * @brief 获取 GPIO 引脚值
 * @param pin GPIO 引脚号
 * @return int 获取结果，0 表示引脚值为 0，1 表示引脚值为 1，-1 表示失败
 */
int gpio_get_value(int pin);

#endif
