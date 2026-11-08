// line_following.c —— 循迹小车主程序
// 按键：PA25 设圈数(按几下=几圈，闪几下)，PB13 开始/暂停
// 编码器：跑完圈数后走"整条边 - 发车点到第一个角"回发车点

#include "ti_msp_dl_config.h"
#include "config.h"
#include "motor.h"
#include "sensor.h"
#include "pid.h"
#include "mpu6050.h"
#include "encoder.h"

static PID_t g_pid;

static void led_on(void)  { DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_PIN); }
static void led_off(void) { DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_PIN); }

static void delay_ms(uint32_t ms)
{
    while (ms--) delay_cycles(CPUCLK_FREQ / 1000UL);
}

static bool start_key_pressed(void) { return (DL_GPIO_readPins(GPIO_START_KEY_PORT, GPIO_START_KEY_PIN) == 0); }
static bool lap_key_pressed(void)   { return (DL_GPIO_readPins(GPIO_LAP_KEY_PORT, GPIO_LAP_KEY_PIN) == 0); }

// 转弯：绕内侧轮转（pivot），内侧轮停、外侧轮转
static void turn_in_place(int32_t dir, int32_t speed)
{
    if (dir < 0) {
        Motor_SetLeft(0);          // 左转：左轮停
        Motor_SetRight(speed);
    } else {
        Motor_SetLeft(speed);      // 右转：右轮停
        Motor_SetRight(0);
    }
}

static void uart_send_char(char c) { DL_UART_Main_transmitDataBlocking(DEBUG_UART_INST, (uint8_t)c); }
static void uart_send_str(const char *s) { while (*s) uart_send_char(*s++); }

static void uart_send_num(int32_t n)
{
    char buf[12];
    int  i = 0;
    if (n < 0) { uart_send_char('-'); n = -n; }
    if (n == 0) { uart_send_char('0'); return; }
    while (n > 0) { buf[i++] = '0' + (char)(n % 10); n /= 10; }
    while (i > 0) { uart_send_char(buf[--i]); }
}

int main(void)
{
    int32_t last_dir     = 1;
    int32_t laps         = 1;
    int32_t press_count  = 0;
    int32_t corner_count = 0;
    bool    turning      = false;
    int32_t black_cnt    = 0;
    float   turn_accum   = 0.0f;
    bool    corner_done  = false;
    int32_t line_stable_cnt = 0;
    bool    paused       = false;
    bool    start_prev   = false;
    bool    lap_prev     = false;

    // 终点阶段：回发车点
    bool    finishing    = false;
    int32_t finish_pulses = 0;
    uint32_t last_enc_r  = 0xFFFF;
    uint32_t last_enc_l  = 0xFFFF;
    int64_t total_pulses = 0;
    int32_t first_corner_pulses = 0;
    bool    first_corner_saved = false;

    uint8_t  dbg_bits[SENSOR_COUNT];
    uint32_t dbg_cnt = 0;
    int32_t  i;

    SYSCFG_DL_init();
    Sensor_Init();
    PID_Init(&g_pid, PID_KP, PID_KI, PID_KD, -BASE_SPEED, BASE_SPEED);
    Encoder_Init();

    // MPU6050 失败用红灯闪错误码：1/2=读写无应答，3=两组闪 WHO_AM_I
    int mpu_err = MPU6050_Init();
    if (mpu_err != 0) {
        uart_send_str("MPU6050 FAIL code:");
        uart_send_num(mpu_err);
        uart_send_str(" who:");
        uart_send_num(g_mpu_who);
        uart_send_str("\r\n");
        while (1) {
            int j;
            if (mpu_err == 3) {
                for (j = 0; j < (g_mpu_who >> 4); j++) { led_on(); delay_ms(250); led_off(); delay_ms(250); }
                delay_ms(1500);
                for (j = 0; j < (g_mpu_who & 0x0F); j++) { led_on(); delay_ms(250); led_off(); delay_ms(250); }
                delay_ms(2500);
            } else {
                for (j = 0; j < mpu_err; j++) { led_on(); delay_ms(250); led_off(); delay_ms(250); }
                delay_ms(1200);
            }
        }
    }
    uart_send_str("MPU6050 OK\r\n");

    // 设圈数：按 PA25 几下 = 几圈，闪几下；按 PB13 开始
    led_on();
    delay_ms(500);
    led_off();

    while (1) {
        if (lap_key_pressed() && !lap_prev) {
            delay_ms(30);
            if (lap_key_pressed()) {
                press_count++;
                if (press_count > 5) press_count = 1;
                laps = press_count;
                for (i = 0; i < laps; i++) { led_on(); delay_ms(100); led_off(); delay_ms(100); }
                while (lap_key_pressed()) { }
                delay_ms(30);
            }
        }
        lap_prev = lap_key_pressed();

        if (start_key_pressed() && !start_prev) {
            delay_ms(30);
            if (start_key_pressed()) break;
        }
        start_prev = start_key_pressed();

        delay_ms(10);
    }

    led_off();
    Motor_Init();
    start_prev = start_key_pressed();

    // 主循环
    while (1) {
        if (start_key_pressed() && !start_prev) {
            delay_ms(30);
            if (start_key_pressed()) {
                paused = !paused;
                if (paused) {
                    Motor_Stop();
                    led_on();
                } else {
                    led_off();
                    PID_Reset(&g_pid);
                    turning         = false;
                    turn_accum      = 0.0f;
                    black_cnt       = 0;
                    corner_done     = false;
                    line_stable_cnt = 0;
                }
            }
        }
        start_prev = start_key_pressed();

        if (paused) { delay_ms(LOOP_PERIOD_MS); continue; }

        // 本次车头转角（度）
        int16_t gyro_z;
        float   d_angle = 0.0f;
        if (MPU6050_ReadGyroZ(&gyro_z)) {
            float a = (float) gyro_z;
            if (a < 0.0f) a = -a;
            d_angle = a * 0.01f / 32.8f;
        }

        int32_t position = Sensor_ReadLine();

        // 编码器脉冲增量（递减计数，无符号减法自动回绕）
        uint32_t enc_r = Encoder_RightCount();
        uint32_t enc_l = Encoder_LeftCount();
        int32_t  dr    = (int32_t)(last_enc_r - enc_r);
        int32_t  dl    = (int32_t)(last_enc_l - enc_l);
        last_enc_r = enc_r;
        last_enc_l = enc_l;
        int32_t avg_pulses = (dr + dl) / 2;
        total_pulses += avg_pulses;

        // 直角弯：连续 50ms 全黑进弯；300ms 冷却防重复触发
        if (Sensor_IsAllBlack()) {
            line_stable_cnt = 0;
            if (!turning && !corner_done) {
                black_cnt++;
                if (black_cnt >= 5) {
                    turning = true;
                    corner_count++;
                    if (!first_corner_saved) {   // 第一个弯：记发车点到它的距离
                        first_corner_pulses = (int32_t) total_pulses;
                        first_corner_saved = true;
                    }
                    turn_accum = 0.0f;
                }
            }
        } else {
            black_cnt = 0;
            line_stable_cnt++;
            if (line_stable_cnt >= 30) corner_done = false;
        }

        if (turning) {
            // 原地转；转满 70° 且黑线回到中段就停
            turn_accum += d_angle;
            if (turn_accum >= 70.0f && Sensor_IsLineCentered()) {
                turning     = false;
                corner_done = true;
                black_cnt   = 0;
                if (corner_count >= laps * 4) {
                    finishing     = true;   // 跑完圈数，进入回程
                    finish_pulses = 0;
                }
            } else {
                // 分级减速
                if (turn_accum >= 68.0f)      turn_in_place(last_dir, TURN_SPEED_SLOW);
                else if (turn_accum >= 50.0f) turn_in_place(last_dir, TURN_SPEED_MID);
                else                          turn_in_place(last_dir, TURN_SPEED_FAST);
            }
        } else if (Sensor_IsLineLost()) {
            // 丢线：低速直行找线
            Motor_SetLeft(BASE_SPEED);
            Motor_SetRight(BASE_SPEED);
        } else {
            // 压线：PID 差速循迹
            int32_t error   = position - LINE_CENTER;
            int32_t abs_err = (error < 0) ? -error : error;
            int32_t speed   = BASE_SPEED - (abs_err / 10);
            if (speed < BASE_SPEED) speed = BASE_SPEED;

            int32_t output   = PID_Update(&g_pid, error);
            int32_t max_diff = speed - 80;
            if (output >  max_diff) output =  max_diff;
            if (output < -max_diff) output = -max_diff;

            Motor_SetLeft((int16_t)(speed + output));
            Motor_SetRight((int16_t)(speed - output));

            // 回程：走够"整条边 - 发车点到第一个角"就停
            if (finishing) {
                finish_pulses += avg_pulses;
                int32_t return_pulses = (int32_t)(EDGE_LEN_CM * ENCODER_PULSES_PER_CM) - first_corner_pulses;
                if (return_pulses < 0) return_pulses = 0;
                if (finish_pulses >= return_pulses) {
                    Motor_Stop();
                    led_on();
                    while (1) { }
                }
            }
        }

        // 调试打印：每 100ms 一次（D=回程累计脉冲，用于校准系数）
        dbg_cnt++;
        if (dbg_cnt >= 10) {
            dbg_cnt = 0;
            Sensor_ReadBits(dbg_bits);
            uart_send_str("S:");
            for (i = 0; i < SENSOR_COUNT; i++) uart_send_char(dbg_bits[i] ? '1' : '0');
            uart_send_str(" P:");
            uart_send_num(position);
            uart_send_str(" C:");
            uart_send_num(corner_count);
            uart_send_str(" A:");
            uart_send_num((int32_t) turn_accum);
            uart_send_str(" D:");
            uart_send_num(finishing ? finish_pulses : 0);
            uart_send_str("\r\n");
        }

        delay_ms(LOOP_PERIOD_MS);
    }
}
