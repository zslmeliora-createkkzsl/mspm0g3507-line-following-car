// motor.c —— TB6612(D24A) 双电机驱动
// 左后轮=A通道，右后轮=D通道；新电机极性反了：前进=IN1=0/IN2=1

#include "motor.h"
#include "ti_msp_dl_config.h"
#include "config.h"

static uint16_t motor_clamp_duty(int16_t speed)
{
    if (speed < 0) speed = -speed;
    if (speed > (int16_t) MAX_SPEED) speed = (int16_t) MAX_SPEED;
    if (speed < 0) speed = 0;
    return (uint16_t) speed;
}

void Motor_Init(void)
{
    DL_GPIO_setPins(GPIO_STBY_PORT, GPIO_STBY_PIN);   // 使能驱动
    DL_TimerG_startCounter(PWM_0_INST);
}

// 左后轮 = A 通道（AIN1=PB18, AIN2=PB19）
static void motor_set_left_dir(bool forward)
{
    if (forward) {
        DL_GPIO_clearPins(GPIO_AIN1_PORT, GPIO_AIN1_PIN);
        DL_GPIO_setPins(GPIO_AIN2_PORT, GPIO_AIN2_PIN);
    } else {
        DL_GPIO_setPins(GPIO_AIN1_PORT, GPIO_AIN1_PIN);
        DL_GPIO_clearPins(GPIO_AIN2_PORT, GPIO_AIN2_PIN);
    }
}

// 右后轮 = D 通道（DIN1=PA15, DIN2=PA8）
static void motor_set_right_dir(bool forward)
{
    if (forward) {
        DL_GPIO_clearPins(GPIO_BIN1_PORT, GPIO_BIN1_PIN);
        DL_GPIO_setPins(GPIO_BIN2_PORT, GPIO_BIN2_PIN);
    } else {
        DL_GPIO_setPins(GPIO_BIN1_PORT, GPIO_BIN1_PIN);
        DL_GPIO_clearPins(GPIO_BIN2_PORT, GPIO_BIN2_PIN);
    }
}

void Motor_SetLeft(int16_t speed)
{
    uint16_t duty = motor_clamp_duty(speed);
    if (speed >= 0) motor_set_left_dir(true);
    else            motor_set_left_dir(false);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWM_0_PERIOD - 1 - duty, PWM_0_C0_IDX);
}

void Motor_SetRight(int16_t speed)
{
    uint16_t duty = motor_clamp_duty(speed);
    if (speed >= 0) motor_set_right_dir(true);
    else            motor_set_right_dir(false);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWM_0_PERIOD - 1 - duty, PWM_0_C1_IDX);
}

void Motor_Stop(void)
{
    // CC = period-1 时占空比为 0
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWM_0_PERIOD - 1, PWM_0_C0_IDX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWM_0_PERIOD - 1, PWM_0_C1_IDX);
}
