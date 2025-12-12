#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

/*
 * 用法: ./copy <old-file> <new-file>
 */
int main(int argc, char **argv)
{
    int fd_old, fd_new;
    char buf[1024];
    int len;

    /* 1. 判断参数有效性 */
    if (argc != 3)
    {
        printf("Usage: %s <old-file> <new-file>\n", argv[0]);
        return -1;
    }

    /* 2. 打开旧文件 (只读模式) */
    // O_RDONLY: Read Only
    fd_old = open(argv[1], O_RDONLY);
    if (fd_old == -1)
    {
        perror("open old file"); // 使用 perror 可以打印具体的系统错误信息
        return -1;
    }

    /* 3. 创建/打开新文件 */
    // O_WRONLY: Write Only (只写)
    // O_CREAT:  Create (如果文件不存在则创建)
    // O_TRUNC:  Truncate (如果文件存在，将其长度截断为0，即清空内容)
    // 0644:     文件权限 (rw-r--r--)
    fd_new = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_new == -1)
    {
        perror("open new file");
        return -1;
    }

    /* 4. 循环：读旧文件 -> 写新文件 */
    // read 返回值：成功读取的字节数，0表示读到文件末尾，-1表示出错
    while ((len = read(fd_old, buf, 1024)) > 0)
    {
        // write 返回值：成功写入的字节数
        if (write(fd_new, buf, len) != len)
        {
            printf("can not write %s\n", argv[2]);
            return -1;
        }
    }

    /* 5. 关闭文件 */
    close(fd_old);
    close(fd_new);

    return 0;
}
