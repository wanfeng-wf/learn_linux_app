#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h> /* mmap 需要的头文件 */

/*
 * 用法: ./copy_mmap <old-file> <new-file>
 */
int main(int argc, char **argv)
{
    int fd_old, fd_new;
    struct stat sb; // 用于获取文件信息（大小）
    char *buf;      // 用于保存映射后的内存地址

    /* 1. 判断参数 */
    if (argc != 3)
    {
        printf("Usage: %s <old-file> <new-file>\n", argv[0]);
        return -1;
    }

    /* 2. 打开旧文件 */
    fd_old = open(argv[1], O_RDONLY);
    if (fd_old == -1)
    {
        perror("open old file");
        return -1;
    }

    /* 3. 获取旧文件的大小 (fstat) */
    // mmap 需要知道映射多少字节，所以必须先获取文件状态
    if (fstat(fd_old, &sb) == -1)
    {
        perror("fstat");
        return -1;
    }
    printf("Old file size: %ld bytes\n", sb.st_size);

    /* 4. 关键步骤：映射旧文件到内存 (mmap) */
    // NULL: 让内核自动选择映射的虚拟地址
    // sb.st_size: 映射长度，即文件大小
    // PROT_READ: 映射区域只读
    // MAP_SHARED: 共享映射，对映射的修改会写回文件（虽然这里是只读）
    // fd_old: 被映射的文件描述符
    // 0: 从文件开头开始映射
    buf = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd_old, 0);
    if (buf == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }
    // for (int i = 0; i < sb.st_size; i++)
    // {
    //     printf("buf[%d]=%c\n", i, buf[i]);
    // }

    /* 5. 创建新文件 */
    fd_new = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_new == -1)
    {
        perror("open new file");
        return -1;
    }

    /* 6. 写新文件 (直接从内存 buf 中拿数据) */
    // 此时 buf 就代表了文件的内容，像操作数组一样简单
    if (write(fd_new, buf, sb.st_size) != sb.st_size)
    {
        printf("can not write %s\n", argv[2]);
        return -1;
    }

    /* 7. 收尾工作 */
    // 关闭映射，虽然进程退出也会自动关闭，但显式关闭是好习惯
    munmap(buf, sb.st_size);
    close(fd_old);
    close(fd_new);

    return 0;
}