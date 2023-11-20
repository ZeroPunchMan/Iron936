#include "main.h"
#include "cl_common.h"
#include "systime.h"
#include "cl_log.h"
#include "usart.h"
#include "adc.h"

void ADC_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ADC1 Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC, ENABLE);

    /* Configure Channel4 as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

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

    /* Convert the ADC1 Channel4 with 239.5 Cycles as sampling time */
    ADC_ChannelConfig(ADC1, ADC_Channel_2 | ADC_Channel_3 | ADC_Channel_4, ADC_SampleTime_239_5Cycles);

    /* ADC1 Calibration */
    ADC_GetCalibrationFactor(ADC1);

    /* Enable the ADC1 peripheral */
    ADC_Cmd(ADC1, ENABLE);

    /* Wait the ADRDY flag */
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY))
        ;
}

static uint16_t adcResult[3] = {0};
void AdcConvert(void)
{
    for (int i = 0; i < 3; i++)
    {
        ADC_StartOfConversion(ADC1);
        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
            ;

        adcResult[i] = ADC_GetConversionValue(ADC1);
    }
}

uint16_t GetAdcResult(AdcChannel_t channel)
{
    return adcResult[channel];
}
