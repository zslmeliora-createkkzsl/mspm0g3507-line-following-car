// pid.c —— PID 实现

#include "pid.h"

void PID_Init(PID_t *pid, float kp, float ki, float kd, int32_t out_min, int32_t out_max)
{
    pid->kp         = kp;
    pid->ki         = ki;
    pid->kd         = kd;
    pid->integral   = 0.0f;
    pid->last_error = 0.0f;
    pid->first      = true;
    pid->out_min    = out_min;
    pid->out_max    = out_max;
}

int32_t PID_Update(PID_t *pid, int32_t error)
{
    float e  = (float) error;
    float p  = pid->kp * e;
    float d  = 0.0f;
    float out;

    if (pid->first) {
        pid->first      = false;
        pid->last_error = e;
    } else {
        d = pid->kd * (e - pid->last_error);
    }
    pid->last_error = e;

    pid->integral += pid->ki * e;
    if (pid->integral >  10000.0f) pid->integral =  10000.0f;   // 积分限幅
    if (pid->integral < -10000.0f) pid->integral = -10000.0f;

    out = p + pid->integral + d;

    if (out > (float) pid->out_max) out = (float) pid->out_max;
    if (out < (float) pid->out_min) out = (float) pid->out_min;
    return (int32_t) out;
}

void PID_Reset(PID_t *pid)
{
    pid->integral   = 0.0f;
    pid->last_error = 0.0f;
    pid->first      = true;
}
