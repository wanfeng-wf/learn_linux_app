#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

// ADC 设备路径
#define IIO_DEV_PATH "/sys/bus/iio/devices/iio:device0"

// 读取文件内容
int read_sysfs(const char *path, char *str)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        perror("Failed to open file");
        return -1;
    }

    if (!fscanf(fp, "%s", str))
    {
        perror("Failed to read file");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int main()
{
    char path[128];
    char str[32];
    float scale = 0.0;
    int raw = 0;
    float volt = 0;
    int ret = 0;

    // 1. 读取比例因子 (Scale)
    snprintf(path, sizeof(path), "%s/in_voltage_scale", IIO_DEV_PATH);
    ret = read_sysfs(path, str);
    if (ret < 0)
    {
        fprintf(stderr, "Error: Failed to read scale from %s\n", path);
        return 1;
    }
    scale = atof(str);

    printf("ADC Scale: %f mV\n", scale);
    printf("Reading ADC values (Press Ctrl+C to stop)...\n\n");

    // 2. 循环读取通道数据
    while (1)
    {
        // 读取通道 0 (SARADC_VIN0)
        // 对应文件: in_voltage0_raw
        snprintf(path, sizeof(path), "%s/in_voltage0_raw", IIO_DEV_PATH);
        ret = read_sysfs(path, str);
        if (ret < 0)
        {
            fprintf(stderr, "Error: Failed to read raw from %s\n", path);
            continue;
        }
        raw = atoi(str);

        // 计算电压: Raw * Scale
        volt = raw * scale;

        // 打印结果
        printf("\r\033[KRaw: %4d | Voltage: %.3f V  ", raw, volt / 1000.0);
        fflush(stdout);

        // 延时 1 秒
        sleep(1);
    }

    return 0;
}
