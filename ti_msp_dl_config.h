// ti_msp_dl_config.h —— 外设配置（手工编写，不用 SysConfig）

#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM0G3507
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POWER_STARTUP_DELAY                                                (16)
#define CPUCLK_FREQ                                                     32000000

// ---- 传感器 8 路输入（YB-MUX04 的 X1~X8）----
#define GPIO_X1_PORT                                                       GPIOB
#define GPIO_X1_PIN                                               DL_GPIO_PIN_16
#define GPIO_X1_IOMUX                                            (IOMUX_PINCM33)

#define GPIO_X2_PORT                                                       GPIOB
#define GPIO_X2_PIN                                                DL_GPIO_PIN_0
#define GPIO_X2_IOMUX                                            (IOMUX_PINCM12)

#define GPIO_X3_PORT                                                       GPIOB
#define GPIO_X3_PIN                                                DL_GPIO_PIN_6
#define GPIO_X3_IOMUX                                            (IOMUX_PINCM23)

#define GPIO_X4_PORT                                                       GPIOB
#define GPIO_X4_PIN                                                DL_GPIO_PIN_7
#define GPIO_X4_IOMUX                                            (IOMUX_PINCM24)

#define GPIO_X5_PORT                                                       GPIOB
#define GPIO_X5_PIN                                                DL_GPIO_PIN_8
#define GPIO_X5_IOMUX                                            (IOMUX_PINCM25)

#define GPIO_X6_PORT                                                       GPIOB
#define GPIO_X6_PIN                                               DL_GPIO_PIN_15
#define GPIO_X6_IOMUX                                            (IOMUX_PINCM32)

#define GPIO_X7_PORT                                                       GPIOB
#define GPIO_X7_PIN                                               DL_GPIO_PIN_17
#define GPIO_X7_IOMUX                                            (IOMUX_PINCM43)

#define GPIO_X8_PORT                                                       GPIOB
#define GPIO_X8_PIN                                               DL_GPIO_PIN_12
#define GPIO_X8_IOMUX                                            (IOMUX_PINCM29)

// ---- PWM（TIMG7，两路电机调速）----
#define PWM_0_INST                                                        TIMG7
// 左轮 PWM = CCP0 = PA17
#define GPIO_PWM_0_C0_PORT                                                 GPIOA
#define GPIO_PWM_0_C0_PIN                                         DL_GPIO_PIN_17
#define GPIO_PWM_0_C0_IOMUX                                      (IOMUX_PINCM39)
#define GPIO_PWM_0_C0_IOMUX_FUNC                     IOMUX_PINCM39_PF_TIMG7_CCP0
#define PWM_0_C0_IDX                                           DL_TIMER_CC_0_INDEX
// 右轮 PWM = CCP1 = PA24
#define GPIO_PWM_0_C1_PORT                                                 GPIOA
#define GPIO_PWM_0_C1_PIN                                         DL_GPIO_PIN_24
#define GPIO_PWM_0_C1_IOMUX                                      (IOMUX_PINCM54)
#define GPIO_PWM_0_C1_IOMUX_FUNC                     IOMUX_PINCM54_PF_TIMG7_CCP1
#define PWM_0_C1_IDX                                           DL_TIMER_CC_1_INDEX

// 周期 3200 => 32MHz/3200 = 10kHz
#define PWM_0_PERIOD                                                       (3200U)
#define PWM_0_MAX_DUTY                                                     (3200U)

// ---- 电机方向引脚（TB6612/D24A）----
// 左轮 A 通道 AIN1=PB18
#define GPIO_AIN1_PORT                                                     GPIOB
#define GPIO_AIN1_PIN                                             DL_GPIO_PIN_18
#define GPIO_AIN1_IOMUX                                           (IOMUX_PINCM44)
// 左轮 A 通道 AIN2=PB19
#define GPIO_AIN2_PORT                                                     GPIOB
#define GPIO_AIN2_PIN                                             DL_GPIO_PIN_19
#define GPIO_AIN2_IOMUX                                           (IOMUX_PINCM45)
// 右轮 D 通道 DIN1=PA15
#define GPIO_BIN1_PORT                                                     GPIOA
#define GPIO_BIN1_PIN                                             DL_GPIO_PIN_15
#define GPIO_BIN1_IOMUX                                           (IOMUX_PINCM37)
// 右轮 D 通道 DIN2=PA8
#define GPIO_BIN2_PORT                                                     GPIOA
#define GPIO_BIN2_PIN                                              DL_GPIO_PIN_8
#define GPIO_BIN2_IOMUX                                           (IOMUX_PINCM19)
// STBY（驱动使能）= PB2
#define GPIO_STBY_PORT                                                     GPIOB
#define GPIO_STBY_PIN                                              DL_GPIO_PIN_2
#define GPIO_STBY_IOMUX                                           (IOMUX_PINCM15)

// ---- 状态灯（板载 LED1 = PA0）----
#define GPIO_LED_PORT                                                      GPIOA
#define GPIO_LED_PIN                                               DL_GPIO_PIN_0
#define GPIO_LED_IOMUX                                            (IOMUX_PINCM1)

// ---- 板载 S2 = PB21（保留，未使用）----
#define GPIO_S2_PORT                                                       GPIOB
#define GPIO_S2_PIN                                               DL_GPIO_PIN_21
#define GPIO_S2_IOMUX                                            (IOMUX_PINCM49)

// ---- 外接设圈数按键 = PA25，上拉输入 ----
#define GPIO_LAP_KEY_PORT                                                  GPIOA
#define GPIO_LAP_KEY_PIN                                           DL_GPIO_PIN_25
#define GPIO_LAP_KEY_IOMUX                                        (IOMUX_PINCM55)

// ---- 调试串口 UART0（TX = PA10）----
#define DEBUG_UART_INST                                                        UART0
#define GPIO_DEBUG_UART_TX_PORT                                               GPIOA
#define GPIO_DEBUG_UART_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_DEBUG_UART_TX_IOMUX                                     (IOMUX_PINCM21)
#define GPIO_DEBUG_UART_TX_IOMUX_FUNC                    IOMUX_PINCM21_PF_UART0_TX

// ---- I2C0（MPU6050，SDA=PA28，SCL=PA31）----
#define I2C_INST                                                            I2C0
#define I2C_BUS_SPEED_HZ                                                  400000
#define GPIO_I2C_SDA_PORT                                                  GPIOA
#define GPIO_I2C_SDA_PIN                                           DL_GPIO_PIN_28
#define GPIO_I2C_IOMUX_SDA                                       (IOMUX_PINCM3)
#define GPIO_I2C_IOMUX_SDA_FUNC                        IOMUX_PINCM3_PF_I2C0_SDA
#define GPIO_I2C_SCL_PORT                                                  GPIOA
#define GPIO_I2C_SCL_PIN                                           DL_GPIO_PIN_31
#define GPIO_I2C_IOMUX_SCL                                       (IOMUX_PINCM6)
#define GPIO_I2C_IOMUX_SCL_FUNC                        IOMUX_PINCM6_PF_I2C0_SCL

// ---- 编码器（单相边沿计数）----
// 右后轮 = PA12，TIMG0 CCP0；左后轮 = PB4，TIMA1 CCP0
#define ENC_RIGHT_INST                                                     TIMG0
#define GPIO_ENC_RIGHT_PORT                                                GPIOA
#define GPIO_ENC_RIGHT_PIN                                        DL_GPIO_PIN_12
#define GPIO_ENC_RIGHT_IOMUX                                      (IOMUX_PINCM34)
#define GPIO_ENC_RIGHT_IOMUX_FUNC                   IOMUX_PINCM34_PF_TIMG0_CCP0

#define ENC_LEFT_INST                                                      TIMA1
#define GPIO_ENC_LEFT_PORT                                                 GPIOB
#define GPIO_ENC_LEFT_PIN                                          DL_GPIO_PIN_4
#define GPIO_ENC_LEFT_IOMUX                                       (IOMUX_PINCM17)
#define GPIO_ENC_LEFT_IOMUX_FUNC                    IOMUX_PINCM17_PF_TIMA1_CCP0

// ---- 开始/暂停按键 = PB13，上拉输入 ----
#define GPIO_START_KEY_PORT                                                GPIOB
#define GPIO_START_KEY_PIN                                         DL_GPIO_PIN_13
#define GPIO_START_KEY_IOMUX                                      (IOMUX_PINCM30)

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_0_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_I2C_0_init(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
