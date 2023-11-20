/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "systime.h"
#include "usart.h"
#include "cl_log.h"
#include "adc.h"
#include "tim.h"
#include "seg_dp.h"
#include "heater.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
    SysTick_Config(32000);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_IOMUX, ENABLE);
    USART_Config();
    ADC_Config();
    TIM_Config();
    SegDp_Init();
    Heater_Init();

    while (1)
    {
        static uint32_t lastTime = 0;
        if (SysTimeSpan(lastTime) >= SYSTIME_SECOND(1))
        {
            lastTime = GetSysTime();
            // CL_LOG_LINE("%ds", GetSysTime() / 1000);

            // AdcConvert();
            // CL_LOG_LINE("adc: %d, %d, %d", GetAdcResult(0), GetAdcResult(1), GetAdcResult(2));
        }
    }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif
