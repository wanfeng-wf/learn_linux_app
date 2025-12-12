#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pwm.h"

#define PWM_EXPORT_PATH "/sys/class/pwm/pwmchip%d/export"
#define PWM_UNEXPORT_PATH "/sys/class/pwm/pwmchip%d/unexport"
#define PWM_ENABLE_PATH "/sys/class/pwm/pwmchip%d/pwm%d/enable"
#define PWM_PERIOD_PATH "/sys/class/pwm/pwmchip%d/pwm%d/period"
#define PWM_DUTY_PATH "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle"

/**
 * @brief 用于向指定的sysfs文件写入值
 * @param path sysfs文件的完整路径
 * @param value 要写入的字符串值
 * @return int 成功返回 0，失败返回 -1
 */
static int write_sysfs_file(const char *path, const char *value)
{
    FILE *fp = fopen(path, "w");
    if (!fp)
    {
        perror("Failed to open file");
        // 打印出具体是哪个文件打开失败，方便调试
        fprintf(stderr, "Path: %s\n", path);
        return -1;
    }
    if (fprintf(fp, "%s", value) < 0)
    {
        perror("Failed to write to file");
        fclose(fp);
        return -1;
    }
    if (fclose(fp) != 0)
    {
        perror("Failed to close file");
        return -1;
    }
    return 0;
}

int pwm_init(int chip_num, int channel_num, unsigned int period_ns)
{
    char path[256];
    char value_str[32];
    // 1. 导出 PWM 通道
    snprintf(path, sizeof(path), PWM_EXPORT_PATH, chip_num);
    snprintf(value_str, sizeof(value_str), "%d", channel_num);
    if (write_sysfs_file(path, value_str) == -1)
    {
        fprintf(stderr, "Error: Failed to export PWM channel %d on chip %d\n", channel_num, chip_num);
        return -1;
    }
    // 2. 设置 PWM 周期
    snprintf(path, sizeof(path), PWM_PERIOD_PATH, chip_num, channel_num);
    snprintf(value_str, sizeof(value_str), "%u", period_ns);
    if (write_sysfs_file(path, value_str) == -1)
    {
        fprintf(stderr, "Error: Failed to set period for PWM channel %d on chip %d\n", channel_num, chip_num);
        // 尝试取消导出以清理
        pwm_deinit(chip_num, channel_num);
        return -1;
    }
    // 3. 使能 PWM 通道
    snprintf(path, sizeof(path), PWM_ENABLE_PATH, chip_num, channel_num);
    if (write_sysfs_file(path, "1") == -1)
    {
        fprintf(stderr, "Error: Failed to enable PWM channel %d on chip %d\n", channel_num, chip_num);
        pwm_deinit(chip_num, channel_num);
        return -1;
    }
    return 0;
}

int pwm_set_duty_cycle(int chip_num, int channel_num, unsigned int duty_cycle_ns)
{
    char path[256];
    char value_str[32];
    snprintf(path, sizeof(path), PWM_DUTY_PATH, chip_num, channel_num);
    snprintf(value_str, sizeof(value_str), "%u", duty_cycle_ns);

    return write_sysfs_file(path, value_str);
}

int pwm_deinit(int chip_num, int channel_num)
{
    char path[256];
    char value_str[32];
    // 1. 关闭 PWM 通道
    snprintf(path, sizeof(path), PWM_ENABLE_PATH, chip_num, channel_num);
    write_sysfs_file(path, "0"); // 即使失败也继续尝试unexport
    // 2. 取消导出 PWM 通道
    snprintf(path, sizeof(path), PWM_UNEXPORT_PATH, chip_num);
    snprintf(value_str, sizeof(value_str), "%d", channel_num);
    if (write_sysfs_file(path, value_str) == -1)
    {
        fprintf(stderr, "Error: Failed to unexport PWM channel %d on chip %d\n", channel_num, chip_num);
        return -1;
    }
    return 0;
}