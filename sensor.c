// sensor.c —— YB-MUX04 八路灰度读取（GPIO 数字模式）

#include "sensor.h"
#include "ti_msp_dl_config.h"
#include "config.h"

typedef struct {
    GPIO_Regs *port;
    uint32_t   pin;
} sensor_pin_t;

static sensor_pin_t g_sensor_pins[SENSOR_COUNT];

static int32_t g_last_position = LINE_CENTER;
static bool    g_line_lost     = false;
static bool    g_all_black     = false;

void Sensor_Init(void)
{
    g_sensor_pins[0].port = GPIO_X1_PORT; g_sensor_pins[0].pin = GPIO_X1_PIN;
    g_sensor_pins[1].port = GPIO_X2_PORT; g_sensor_pins[1].pin = GPIO_X2_PIN;
    g_sensor_pins[2].port = GPIO_X3_PORT; g_sensor_pins[2].pin = GPIO_X3_PIN;
    g_sensor_pins[3].port = GPIO_X4_PORT; g_sensor_pins[3].pin = GPIO_X4_PIN;
    g_sensor_pins[4].port = GPIO_X5_PORT; g_sensor_pins[4].pin = GPIO_X5_PIN;
    g_sensor_pins[5].port = GPIO_X6_PORT; g_sensor_pins[5].pin = GPIO_X6_PIN;
    g_sensor_pins[6].port = GPIO_X7_PORT; g_sensor_pins[6].pin = GPIO_X7_PIN;
    g_sensor_pins[7].port = GPIO_X8_PORT; g_sensor_pins[7].pin = GPIO_X8_PIN;

    g_last_position = LINE_CENTER;
    g_line_lost     = false;
    g_all_black     = false;
}

void Sensor_ReadBits(uint8_t *bits)
{
    uint8_t i;
    for (i = 0; i < SENSOR_COUNT; i++) {
        uint8_t raw = (DL_GPIO_readPins(g_sensor_pins[i].port, g_sensor_pins[i].pin) != 0) ? 1 : 0;
        if (SENSOR_ACTIVE_LEVEL == 1) bits[i] = raw;
        else                          bits[i] = (raw == 0) ? 1 : 0;
    }
    if (SENSOR_INVERT_ORDER) {
        for (i = 0; i < SENSOR_COUNT / 2; i++) {
            uint8_t tmp = bits[i];
            bits[i] = bits[SENSOR_COUNT - 1 - i];
            bits[SENSOR_COUNT - 1 - i] = tmp;
        }
    }
}

int32_t Sensor_ReadLine(void)
{
    uint8_t bits[SENSOR_COUNT];
    int32_t avg = 0, sum = 0;
    uint8_t i;

    Sensor_ReadBits(bits);
    for (i = 0; i < SENSOR_COUNT; i++) {
        if (bits[i]) {
            avg += (int32_t) i * 1000L;
            sum += 1;
        }
    }

    if (sum == 0) {
        g_line_lost = true;
        g_all_black = false;
        return g_last_position;
    }

    g_line_lost = false;
    g_all_black = (sum == SENSOR_COUNT);   // 8 路全黑 = 直角弯/横线

    g_last_position = avg / sum;
    return g_last_position;
}

bool Sensor_IsLineLost(void)    { return g_line_lost; }
bool Sensor_IsAllBlack(void)    { return g_all_black; }

bool Sensor_IsLineCentered(void)
{
    uint8_t bits[SENSOR_COUNT];
    int32_t avg = 0, sum = 0, position;
    uint8_t i;

    Sensor_ReadBits(bits);
    for (i = 0; i < SENSOR_COUNT; i++) {
        if (bits[i]) {
            avg += (int32_t) i * 1000L;
            sum += 1;
        }
    }

    if (sum < 1 || sum > 7) return false;   // 不能全黑(8)也不能全白(0)

    position = avg / sum;
    return (position > 2500 && position < 4500);   // 线在中段
}
