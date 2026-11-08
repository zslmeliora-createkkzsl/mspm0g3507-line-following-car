// motor.h —— TB6612(D24A) 双电机驱动接口

#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include <stdbool.h>

void Motor_Init(void);
void Motor_SetLeft(int16_t speed);
void Motor_SetRight(int16_t speed);
void Motor_Stop(void);

#endif
