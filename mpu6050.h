// mpu6050.h —— MPU6050 陀螺仪驱动（只用 Z 轴测车头转角）

#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include <stdbool.h>

// 返回 0=成功；1=写失败 2=读失败 3=WHO_AM_I 不对（见 g_mpu_who）
int MPU6050_Init(void);

extern uint8_t g_mpu_who;

// 读 Z 轴角速度原始值（±1000°/s 下 32.8 LSB = 1°/s）
bool MPU6050_ReadGyroZ(int16_t *gyro);

#endif
