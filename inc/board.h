#include "hk32f030m_gpio.h"

/**************************
 * 数码管 端口配置,从f->a->g->dp
 *************************/
#define SEG_F_PORT (GPIOB)
#define SEG_F_PIN (GPIO_Pin_5)

#define SEG_E_PORT (GPIOC)
#define SEG_E_PIN (GPIO_Pin_7)

#define SEG_D_PORT (GPIOC)
#define SEG_D_PIN (GPIO_Pin_5)

#define SEG_C_PORT (GPIOD)
#define SEG_C_PIN (GPIO_Pin_4)

#define SEG_B_PORT (GPIOD)
#define SEG_B_PIN (GPIO_Pin_6)

#define SEG_A_PORT (GPIOB)
#define SEG_A_PIN (GPIO_Pin_4)

#define SEG_G_PORT (GPIOA)
#define SEG_G_PIN (GPIO_Pin_2)

// #define SEG_DP_PORT (GPIOF)
// #define SEG_DP_PIN (GPIO_Pin_5)
