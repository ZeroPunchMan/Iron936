/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "systime.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_IT_TX_Def Usart1TxBuffer;

/* Private function prototypes -----------------------------------------------*/
static void USART_Config(void);
static void USART1_IT_Send(uint8_t *pData, uint16_t len);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
    /* Initialize LEDs available on STM320518-EVAL board ************************/
    HK_EVAL_LEDInit(LED1);

    SysTick_Config(32000);
    /* USART configuration */
    USART_Config();

    /* Set USART1 TxBuffer Status */
    Usart1TxBuffer.FinishedFlag = SET;

    while (1)
    {
        static uint32_t lastTime = 0;
        if(SysTimeSpan(lastTime) >= 5000)
        {
            lastTime = GetSysTime();
        }
        /* Test */
        if (Usart1TxBuffer.FinishedFlag == SET)
        {
            HK_EVAL_LEDOn(LED1);
            USART1_IT_Send("USART1 IT Send String Testing is OK!\r\n", sizeof("USART1 IT Send String Testing is OK!\r\n") - 1);
        }
    }
}

/**
 * @brief Configure the USART Device
 * @param  None
 * @retval None
 */
static void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);

    /* Enable USART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_1);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* USARTx configured as follow:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - Stop Bit = 1 Stop Bit
    - Parity = No Parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);

    /* Configre NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief  UART Send data with TXE intterupt.
 * @param  None
 * @retval None
 */
static void USART1_IT_Send(uint8_t *pData, uint16_t len)
{
    unsigned char i;

    Usart1TxBuffer.Len = len;
    Usart1TxBuffer.Index = 0;
    Usart1TxBuffer.FinishedFlag = RESET;

    /* Copy data to transfer buffer */
    for (i = 0; i < len; i++)
    {
        Usart1TxBuffer.Data[i] = *(pData++);
    }

    /* Enable transfer empty interrupt */
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

    /* Enable transfer complete interrupt */
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);
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

/**
 * @}
 */

/**
 * @}
 */
