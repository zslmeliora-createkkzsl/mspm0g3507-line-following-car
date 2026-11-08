// mpu6050.c —— MPU6050 驱动（I2C0，地址 0x68）

#include "mpu6050.h"
#include "ti_msp_dl_config.h"

#define MPU6050_ADDR         0x68
#define REG_PWR_MGMT_1       0x6B
#define REG_GYRO_CONFIG      0x1B
#define REG_GYRO_ZOUT_H      0x47
#define REG_WHO_AM_I         0x75
#define I2C_TIMEOUT          50000U

uint8_t g_mpu_who = 0;

static bool i2c_wait_idle(void)
{
    uint32_t t = I2C_TIMEOUT;
    while (!(DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_IDLE)) {
        if (--t == 0) return false;
    }
    return true;
}

static bool i2c_wait_busy_done(void)
{
    uint32_t t = I2C_TIMEOUT;
    while (DL_I2C_getControllerStatus(I2C_INST) & DL_I2C_CONTROLLER_STATUS_BUSY) {
        if (--t == 0) return false;
    }
    return true;
}

static bool i2c_wait_rx_ready(void)
{
    uint32_t t = I2C_TIMEOUT;
    while (DL_I2C_isControllerRXFIFOEmpty(I2C_INST)) {
        if (--t == 0) return false;
    }
    return true;
}

static bool mpu6050_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    if (!i2c_wait_idle()) return false;
    DL_I2C_fillControllerTXFIFO(I2C_INST, buf, 2);
    DL_I2C_startControllerTransfer(I2C_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 2);
    if (!i2c_wait_busy_done()) return false;
    if (!i2c_wait_idle()) return false;
    return true;
}

// 读寄存器：写地址(不发STOP) + 重复起始 + 读1字节(发STOP)
// STOP_DISABLE 之后只能等 BUSY 清，不能等 IDLE
static bool mpu6050_read_reg(uint8_t reg, uint8_t *out)
{
    uint8_t data = 0;
    if (!i2c_wait_idle()) return false;

    DL_I2C_fillControllerTXFIFO(I2C_INST, &reg, 1);
    DL_I2C_startControllerTransferAdvanced(I2C_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 1,
        DL_I2C_CONTROLLER_START_ENABLE, DL_I2C_CONTROLLER_STOP_DISABLE,
        DL_I2C_CONTROLLER_ACK_DISABLE);
    if (!i2c_wait_busy_done()) return false;

    DL_I2C_startControllerTransferAdvanced(I2C_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_RX, 1,
        DL_I2C_CONTROLLER_START_ENABLE, DL_I2C_CONTROLLER_STOP_ENABLE,
        DL_I2C_CONTROLLER_ACK_DISABLE);
    if (!i2c_wait_rx_ready()) return false;
    data = DL_I2C_receiveControllerData(I2C_INST);
    if (!i2c_wait_idle()) return false;

    *out = data;
    return true;
}

int MPU6050_Init(void)
{
    uint8_t who;

    if (!mpu6050_write_reg(REG_PWR_MGMT_1, 0x00)) return 1;   // 唤醒
    delay_cycles((CPUCLK_FREQ / 1000UL) * 50);

    if (!mpu6050_write_reg(REG_GYRO_CONFIG, 0x10)) return 1;  // ±1000°/s

    if (!mpu6050_read_reg(REG_WHO_AM_I, &who)) return 2;
    switch (who) {
        case 0x68:   // MPU6050/6000
        case 0x70:   // MPU6500
        case 0x71:   // MPU9250
        case 0x73:   // MPU9255
            return 0;
        default:
            g_mpu_who = who;
            return 3;
    }
}

bool MPU6050_ReadGyroZ(int16_t *gyro)
{
    uint8_t reg = REG_GYRO_ZOUT_H;
    uint8_t buf[2] = { 0, 0 };
    int i;

    if (!i2c_wait_idle()) return false;

    DL_I2C_fillControllerTXFIFO(I2C_INST, &reg, 1);
    DL_I2C_startControllerTransferAdvanced(I2C_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, 1,
        DL_I2C_CONTROLLER_START_ENABLE, DL_I2C_CONTROLLER_STOP_DISABLE,
        DL_I2C_CONTROLLER_ACK_DISABLE);
    if (!i2c_wait_busy_done()) return false;

    DL_I2C_startControllerTransferAdvanced(I2C_INST, MPU6050_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_RX, 2,
        DL_I2C_CONTROLLER_START_ENABLE, DL_I2C_CONTROLLER_STOP_ENABLE,
        DL_I2C_CONTROLLER_ACK_DISABLE);
    for (i = 0; i < 2; i++) {
        if (!i2c_wait_rx_ready()) return false;
        buf[i] = DL_I2C_receiveControllerData(I2C_INST);
    }
    if (!i2c_wait_idle()) return false;

    *gyro = (int16_t)((buf[0] << 8) | buf[1]);
    return true;
}
