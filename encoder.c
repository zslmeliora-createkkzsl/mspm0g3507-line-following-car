// encoder.c —— 编码器驱动（单相边沿计数，递减计数）
// 右后轮：TIMG0 CCP0 = PA12；左后轮：TIMA1 CCP0 = PB4

#include "encoder.h"
#include "ti_msp_dl_config.h"

static const DL_Timer_ClockConfig gEncClockCfg = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 0U,
};

static const DL_Timer_CompareConfig gEncCompareCfg = {
    .compareMode    = DL_TIMER_COMPARE_MODE_EDGE_COUNT,        // 递减计数
    .count          = 0xFFFF,                                  // LOAD=65535，数到 0 回绕
    .edgeDetectMode = DL_TIMER_COMPARE_EDGE_DETECTION_MODE_EDGE, // 双沿
    .inputChan      = DL_TIMER_INPUT_CHAN_0,
    .inputInvMode   = DL_TIMER_CC_INPUT_INV_NOINVERT,
    .startTimer     = DL_TIMER_START,
};

void Encoder_Init(void)
{
    DL_TimerG_enablePower(ENC_RIGHT_INST);
    DL_TimerG_setClockConfig(ENC_RIGHT_INST, (DL_Timer_ClockConfig *) &gEncClockCfg);
    DL_TimerG_initCompareMode(ENC_RIGHT_INST, (DL_Timer_CompareConfig *) &gEncCompareCfg);
    DL_TimerG_enableClock(ENC_RIGHT_INST);

    DL_TimerA_enablePower(ENC_LEFT_INST);
    DL_TimerA_setClockConfig(ENC_LEFT_INST, (DL_Timer_ClockConfig *) &gEncClockCfg);
    DL_TimerA_initCompareMode(ENC_LEFT_INST, (DL_Timer_CompareConfig *) &gEncCompareCfg);
    DL_TimerA_enableClock(ENC_LEFT_INST);
}

uint32_t Encoder_RightCount(void)
{
    return DL_TimerG_getTimerCount(ENC_RIGHT_INST);
}

uint32_t Encoder_LeftCount(void)
{
    return DL_TimerA_getTimerCount(ENC_LEFT_INST);
}
