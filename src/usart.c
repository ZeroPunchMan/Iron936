#include "usart.h"

CL_QUEUE_DEF_INIT(usart1SendQueue, 256, uint8_t, );

void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable USART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

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
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);

    /* Configre NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);
}

static inline void EnableTxe(USART_TypeDef *Usartx)
{
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

//--------------------------------------
#include "stdio.h"
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
    x = x;
}

int fputc(int ch, FILE *f)
{
    uint8_t data = ch;
    CL_QueueAdd(&usart1SendQueue, &data);
    EnableTxe(USART1);
    return ch;
}
