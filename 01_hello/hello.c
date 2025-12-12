#include <stdio.h>

int main()
{
    printf("Hello, TaiShan-Pi (RK3566)!\n");
#ifdef __aarch64__
    printf("I was compiled for ARM64.\n");
#else
    printf("I was NOT compiled for ARM64.\n");
#endif
    return 0;
}
