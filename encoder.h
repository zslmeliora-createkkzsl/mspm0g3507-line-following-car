// encoder.h —— 两个电机编码器（单相边沿计数测速测距）

#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

void Encoder_Init(void);
uint32_t Encoder_RightCount(void);   // 递减计数，16 位自动回绕
uint32_t Encoder_LeftCount(void);

#endif
