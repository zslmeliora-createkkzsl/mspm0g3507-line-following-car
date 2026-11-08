// pid.h —— 通用 PID

#ifndef PID_H
#define PID_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float    kp, ki, kd;
    float    integral;
    float    last_error;
    bool     first;
    int32_t  out_min;
    int32_t  out_max;
} PID_t;

void PID_Init(PID_t *pid, float kp, float ki, float kd, int32_t out_min, int32_t out_max);
int32_t PID_Update(PID_t *pid, int32_t error);
void PID_Reset(PID_t *pid);

#endif
