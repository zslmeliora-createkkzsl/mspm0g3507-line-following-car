// sensor.h —— YB-MUX04 八路灰度读取

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <stdbool.h>

#define SENSOR_COUNT        8

void Sensor_Init(void);
void Sensor_ReadBits(uint8_t *bits);      // 1=压黑线
int32_t Sensor_ReadLine(void);            // 加权位置 0~7000，3500 居中
bool Sensor_IsLineLost(void);
bool Sensor_IsAllBlack(void);             // 8 路全黑
bool Sensor_IsLineCentered(void);         // 黑线在中段（转弯判停用）

#endif
