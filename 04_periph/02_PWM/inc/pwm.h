#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H
/**
 * @brief 初始化指定的PWM通道，该函数会导出PWM通道，设置其周期，并使能它。
 * @param chip_num PWM 控制器号
 * @param channel_num PWM 通道号
 * @param period_ns PWM周期的纳秒数
 * @return int 成功返回 0，失败返回 -1
 */
int pwm_init(int chip_num, int channel_num, unsigned int period_ns);

/**
 * @brief 设置PWM通道的占空比
 * @param chip_num PWM 控制器号
 * @param channel_num PWM 通道号
 * @param duty_cycle_ns 占空比的纳秒数
 * @return int 成功返回 0，失败返回 -1
 */
int pwm_set_duty_cycle(int chip_num, int channel_num, unsigned int duty_cycle_ns);

/**
 * @brief 反初始化（关闭）指定的PWM通道，该函数会先禁用PWM通道，然后取消导出，释放资源。
 * @param chip_num PWM芯片号
 * @param channel_num PWM通道号
 * @return int 成功返回 0，失败返回 -1
 */
int pwm_deinit(int chip_num, int channel_num);
#endif // PWM_CONTROL_H