#include "main.h"
#include "cl_common.h"
#include "systime.h"
#include "cl_log.h"
#include "usart.h"
#include "adc.h"

__IO uint16_t ADC_Channel2_ConvertedValue;
__IO uint16_t ADC_Channel3_ConvertedValue;
__IO uint16_t ADC_Channel4_ConvertedValue;

__IO uint8_t ADC_SEQ_FLAG = RESET;

void ADC_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* GPIO Periph clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

    /* ADC1 Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC, ENABLE);

    /* Configure ADC1 Channel2,Channel3 and Channel4 as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* ADC1 DeInit */
    ADC_DeInit(ADC1);

    /* Initialize ADC1 structure */
    ADC_StructInit(&ADC_InitStructure);

    /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits */
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* Enable ADC1 end of convert end interrupt	*/
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

    /* Enable sequence convert end interrupt */
    ADC_ITConfig(ADC1, ADC_IT_EOSEQ, ENABLE);

    /* Convert the ADC1 Channel2, Channel3 and Channel4 with 239.5 Cycles as sampling time */
    ADC_ChannelConfig(ADC1, ADC_Channel_2 | ADC_Channel_3 | ADC_Channel_4, ADC_SampleTime_239_5Cycles);

    /* ADC1 Calibration */
    ADC_GetCalibrationFactor(ADC1);

    /* Enable the ADC1 peripheral */
    ADC_Cmd(ADC1, ENABLE);

    /* Wait the ADRDY flag */
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY))
        ;
}

void AdcConvert(void)
{
    ADC_SEQ_FLAG = RESET;
    /* ADC1 regular Software Start Conv */
    ADC_StartOfConversion(ADC1);
    while (ADC_SEQ_FLAG == RESET)
        ;
}

uint16_t GetAdcResult(AdcChannel_t channel)
{
    switch (channel)
    {
    case AdcChann_Heater:
        return ADC_Channel2_ConvertedValue;
    case AdcChann_SleepDelay:
        return ADC_Channel4_ConvertedValue;
    case AdcChann_TargetTemp:
        return ADC_Channel3_ConvertedValue;
    }
    return 0;
}
