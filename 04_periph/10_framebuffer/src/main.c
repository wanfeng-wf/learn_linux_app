#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>

// ST7735S 通常使用 RGB565 格式 (16位)
// 红色: 5位, 绿色: 6位, 蓝色: 5位
#define RGB565(r, g, b) (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))

int main()
{
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    uint16_t *fbp       = 0; // 使用 16位指针，方便操作 RGB565

    // 1. 打开 Framebuffer 设备
    fbfd = open("/dev/fb1", O_RDWR);
    if (fbfd == -1)
    {
        perror("Error: cannot open framebuffer device");
        return 1;
    }
    printf("The framebuffer device was opened successfully.\n");

    // 2. 获取屏幕固定参数 (显存长度等)
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1)
    {
        perror("Error reading fixed information");
        return 1;
    }

    // 3. 获取屏幕可变参数 (分辨率、色深)
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1)
    {
        perror("Error reading variable information");
        return 1;
    }

    printf("Screen Info: %dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // 4. 计算显存大小并映射到内存
    // 显存大小 = 宽 * 高 * 字节数 (16位是2字节)
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    fbp = (uint16_t *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((intptr_t)fbp == -1)
    {
        perror("Error: failed to map framebuffer device to memory");
        return 1;
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

    // 5. 开始绘图
    // 这里的操作是直接写内存，速度非常快
    int x, y;
    long int location = 0;

    for (y = 0; y < vinfo.yres; y++)
    {
        for (x = 0; x < vinfo.xres; x++)
        {

            // 计算当前像素在数组中的下标 (线性偏移)
            location = x + y * vinfo.xres;

            uint16_t color;

            // --- 绘制白色边框 (检查偏移) ---
            if (x < 2 || x >= vinfo.xres - 2 || y < 2 || y >= vinfo.yres - 2)
            {
                color = RGB565(255, 255, 255); // 白
            }
            // --- 绘制三色条纹 (检查 RGB 顺序) ---
            else if (y < vinfo.yres / 3)
            {
                color = RGB565(255, 0, 0); // 上部：红
            }
            else if (y < (vinfo.yres * 2) / 3)
            {
                color = RGB565(0, 255, 0); // 中部：绿
            }
            else
            {
                color = RGB565(0, 0, 255); // 下部：蓝
            }

            // 直接写入映射后的内存
            *(fbp + location) = color;
        }
    }

    // 6. 清理
    munmap(fbp, screensize);
    close(fbfd);

    printf("Test finished. Please check the screen.\n");
    return 0;
}