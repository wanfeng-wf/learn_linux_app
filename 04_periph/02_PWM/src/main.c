#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "pwm.h"

#define PWM_CHIP 1
#define PWM_CHANNEL 0
#define PERIOD_NS 1000000
#define MIN_DUTY_CYCLE_NS 0
#define MAX_DUTY_CYCLE_NS 1000000
#define DUTY_CYCLE_STEP 10000
#define SLEEP_US 50000

// 使用 volatile sig_atomic_t 来安全地从信号处理程序中修改标志
volatile sig_atomic_t keep_running = 1;
void handle_sigint(int sig)
{
    keep_running = 0;
}

int main()
{
    // 注册信号处理函数，用于捕获 Ctrl+C
    signal(SIGINT, handle_sigint);

    if (pwm_init(PWM_CHIP, PWM_CHANNEL, PERIOD_NS) != 0)
    {
        fprintf(stderr, "PWM initialization failed. Exiting.\n");
        return -1;
    }
    printf("PWM initialized.Press Ctrl+C to stop.\n");

    int direction = 1;
    unsigned int duty_cycle_ns = 0;
    while (keep_running)
    {
        // 更新占空比
        duty_cycle_ns += DUTY_CYCLE_STEP * direction;
        if (duty_cycle_ns >= MAX_DUTY_CYCLE_NS)
        {
            duty_cycle_ns = MAX_DUTY_CYCLE_NS;
            direction = -1;
        }
        else if (duty_cycle_ns <= MIN_DUTY_CYCLE_NS)
        {
            duty_cycle_ns = MIN_DUTY_CYCLE_NS;
            direction = 1;
        }
        // 设置新的占空比
        if (pwm_set_duty_cycle(PWM_CHIP, PWM_CHANNEL, duty_cycle_ns) != 0)
        {
            fprintf(stderr, "Failed to set duty cycle. Stopping.\n");
            break; // 出错则退出循环
        }
        usleep(SLEEP_US);
    }

    if (pwm_deinit(PWM_CHIP, PWM_CHANNEL) != 0)
    {
        fprintf(stderr, "PWM deinitialization failed.\n");
        return -1;
    }
    printf("\nPWM deinitialized successfully.\n");
    return 0;
}