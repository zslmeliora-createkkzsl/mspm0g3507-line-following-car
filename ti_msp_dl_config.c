// ti_msp_dl_config.c —— 外设初始化（手工编写，不用 SysConfig）

#include "ti_msp_dl_config.h"

SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_PWM_0_init();
    SYSCFG_DL_UART_0_init();
    SYSCFG_DL_I2C_0_init();
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerG_reset(PWM_0_INST);
    DL_TimerG_reset(ENC_RIGHT_INST);    // TIMG0 编码器
    DL_TimerA_reset(ENC_LEFT_INST);     // TIMA1 编码器
    DL_UART_Main_reset(DEBUG_UART_INST);
    DL_I2C_reset(I2C_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerG_enablePower(PWM_0_INST);
    DL_TimerG_enablePower(ENC_RIGHT_INST);
    DL_TimerA_enablePower(ENC_LEFT_INST);
    DL_UART_Main_enablePower(DEBUG_UART_INST);
    DL_I2C_enablePower(I2C_INST);

    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{
    // 传感器 8 路输入
    DL_GPIO_initDigitalInput(GPIO_X1_IOMUX);
    DL_GPIO_initDigitalInput(GPIO_X2_IOMUX);
    DL_GPIO_initDigitalInput(GPIO_X3_IOMUX);
    DL_GPIO_initDigitalInput(GPIO_X4_IOMUX);
    DL_GPIO_initDigitalInput(GPIO_X5_IOMUX);
    DL_GPIO_initDigitalInput(GPIO_X6_IOMUX);
    DL_GPIO_initDigitalInput(GPIO_X7_IOMUX);
    DL_GPIO_initDigitalInput(GPIO_X8_IOMUX);

    // 状态灯（PA0，低电平点亮）
    DL_GPIO_initDigitalOutput(GPIO_LED_IOMUX);
    DL_GPIO_setPins(GPIO_LED_PORT, GPIO_LED_PIN);   // 初始熄灭
    DL_GPIO_enableOutput(GPIO_LED_PORT, GPIO_LED_PIN);

    // 板载 S2（PB21，未使用）
    DL_GPIO_initDigitalInput(GPIO_S2_IOMUX);
    DL_GPIO_setDigitalInternalResistor(GPIO_S2_IOMUX, DL_GPIO_RESISTOR_PULL_UP);

    // 设圈数按键（PA25，上拉输入）
    DL_GPIO_initDigitalInput(GPIO_LAP_KEY_IOMUX);
    DL_GPIO_setDigitalInternalResistor(GPIO_LAP_KEY_IOMUX, DL_GPIO_RESISTOR_PULL_UP);

    // 开始/暂停按键（PB13，上拉输入）
    DL_GPIO_initDigitalInput(GPIO_START_KEY_IOMUX);
    DL_GPIO_setDigitalInternalResistor(GPIO_START_KEY_IOMUX, DL_GPIO_RESISTOR_PULL_UP);

    // 编码器输入（PA12=TIMG0 CCP0，PB4=TIMA1 CCP0）
    DL_GPIO_initPeripheralInputFunction(GPIO_ENC_RIGHT_IOMUX, GPIO_ENC_RIGHT_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_ENC_LEFT_IOMUX, GPIO_ENC_LEFT_IOMUX_FUNC);

    // 调试串口 TX（PA10）
    DL_GPIO_initPeripheralOutputFunction(GPIO_DEBUG_UART_TX_IOMUX, GPIO_DEBUG_UART_TX_IOMUX_FUNC);

    // I2C0（SDA=PA28，SCL=PA31，开漏）
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_IOMUX_SDA,
        GPIO_I2C_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_IOMUX_SCL,
        GPIO_I2C_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_IOMUX_SCL);

    // 两路 PWM 输出
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_0_C0_IOMUX, GPIO_PWM_0_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_0_C0_PORT, GPIO_PWM_0_C0_PIN);

    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_0_C1_IOMUX, GPIO_PWM_0_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_0_C1_PORT, GPIO_PWM_0_C1_PIN);

    // 电机方向引脚
    DL_GPIO_initDigitalOutput(GPIO_AIN1_IOMUX);
    DL_GPIO_initDigitalOutput(GPIO_AIN2_IOMUX);
    DL_GPIO_initDigitalOutput(GPIO_BIN1_IOMUX);
    DL_GPIO_initDigitalOutput(GPIO_BIN2_IOMUX);
    DL_GPIO_enableOutput(GPIO_AIN1_PORT, GPIO_AIN1_PIN);
    DL_GPIO_enableOutput(GPIO_AIN2_PORT, GPIO_AIN2_PIN);
    DL_GPIO_enableOutput(GPIO_BIN1_PORT, GPIO_BIN1_PIN);
    DL_GPIO_enableOutput(GPIO_BIN2_PORT, GPIO_BIN2_PIN);

    // STBY（驱动使能），上电先待机
    DL_GPIO_initDigitalOutput(GPIO_STBY_IOMUX);
    DL_GPIO_clearPins(GPIO_STBY_PORT, GPIO_STBY_PIN);
    DL_GPIO_enableOutput(GPIO_STBY_PORT, GPIO_STBY_PIN);
}

SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    // 默认时钟：SYSOSC 32MHz，不分频
    DL_SYSCTL_disableHFXT();
    DL_SYSCTL_disableSYSPLL();
}

// PWM：TIMG7 边沿对齐，周期 3200 => 10kHz
static const DL_TimerG_ClockConfig gPWM_0ClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 0U,   // 32MHz / (1 * (0+1)) = 32MHz
};

static const DL_TimerG_PWMConfig gPWM_0Config = {
    .pwmMode            = DL_TIMER_PWM_MODE_EDGE_ALIGN,
    .period             = PWM_0_PERIOD,
    .isTimerWithFourCC  = false,
    .startTimer         = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_0_init(void)
{
    DL_TimerG_setClockConfig(PWM_0_INST, (DL_TimerG_ClockConfig *) &gPWM_0ClockConfig);
    DL_TimerG_initPWMMode(PWM_0_INST, (DL_TimerG_PWMConfig *) &gPWM_0Config);

    DL_TimerG_setCounterControl(PWM_0_INST,
        DL_TIMER_CZC_CCCTL0_ZCOND, DL_TIMER_CAC_CCCTL0_ACOND, DL_TIMER_CLC_CCCTL0_LCOND);

    // 左轮（CC0）。INIT_VAL_LOW 下占空比 = (period-1-CC)/period
    DL_TimerG_setCaptureCompareOutCtl(PWM_0_INST,
        DL_TIMER_CC_OCTL_INIT_VAL_LOW, DL_TIMER_CC_OCTL_INV_OUT_DISABLED,
        DL_TIMER_CC_OCTL_SRC_FUNCVAL, PWM_0_C0_IDX);
    DL_TimerG_setCaptCompUpdateMethod(PWM_0_INST,
        DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, PWM_0_C0_IDX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWM_0_PERIOD - 1, PWM_0_C0_IDX);   // 初始停

    // 右轮（CC1）
    DL_TimerG_setCaptureCompareOutCtl(PWM_0_INST,
        DL_TIMER_CC_OCTL_INIT_VAL_LOW, DL_TIMER_CC_OCTL_INV_OUT_DISABLED,
        DL_TIMER_CC_OCTL_SRC_FUNCVAL, PWM_0_C1_IDX);
    DL_TimerG_setCaptCompUpdateMethod(PWM_0_INST,
        DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, PWM_0_C1_IDX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, PWM_0_PERIOD - 1, PWM_0_C1_IDX);   // 初始停

    DL_TimerG_enableClock(PWM_0_INST);
    DL_TimerG_setCCPDirection(PWM_0_INST, DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT);
}

// 调试串口 UART0（TX=PA10），115200
static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_0Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_0_init(void)
{
    DL_UART_Main_setClockConfig(DEBUG_UART_INST, (DL_UART_Main_ClockConfig *) &gUART_0ClockConfig);
    DL_UART_Main_init(DEBUG_UART_INST, (DL_UART_Main_Config *) &gUART_0Config);
    DL_UART_Main_setOversampling(DEBUG_UART_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(DEBUG_UART_INST, 17, 23);   // 32MHz -> 115200
    DL_UART_Main_enable(DEBUG_UART_INST);
}

// I2C0（MPU6050，400kHz）
static const DL_I2C_ClockConfig gI2CClockConfig = {
    .clockSel    = DL_I2C_CLOCK_BUSCLK,
    .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_I2C_0_init(void)
{
    DL_I2C_setClockConfig(I2C_INST, (DL_I2C_ClockConfig *) &gI2CClockConfig);
    DL_I2C_disableAnalogGlitchFilter(I2C_INST);

    DL_I2C_resetControllerTransfer(I2C_INST);
    DL_I2C_setTimerPeriod(I2C_INST, 7);   // 400kHz
    DL_I2C_setControllerTXFIFOThreshold(I2C_INST, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setControllerRXFIFOThreshold(I2C_INST, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    DL_I2C_enableControllerClockStretching(I2C_INST);
    DL_I2C_enableController(I2C_INST);
}
