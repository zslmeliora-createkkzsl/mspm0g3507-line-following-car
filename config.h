// config.h —— 循迹小车可调参数

#ifndef CONFIG_H
#define CONFIG_H

// 传感器
#define SENSOR_ACTIVE_LEVEL   1   // 1=压黑线输出高，0=输出低
#define SENSOR_INVERT_ORDER   0   // 左右反了改 1

// 车速（PWM 0~3200）
#define BASE_SPEED            480   // 直行基础速度
#define MAX_SPEED             2400  // 单轮最大速度（12V 别超）
#define TURN_SPEED_FAST       720   // 直角弯起步
#define TURN_SPEED_MID        640   // 直角弯中段
#define TURN_SPEED_SLOW       560   // 直角弯末段

// PID
#define PID_KP                0.40f
#define PID_KI                0.0f
#define PID_KD                0.3f

// 循迹
#define LINE_CENTER           3500L  // 8 路加权位置中心
#define LOOP_PERIOD_MS        10     // 控制周期

// 编码器测距
#define ENCODER_PULSES_PER_CM 26     // 每厘米脉冲数（实测校准）
#define EDGE_LEN_CM           100    // 场地边长（cm）

#endif
