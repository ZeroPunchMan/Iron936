#pragma once

#include "cl_common.h"


typedef enum
{
    AdcChann_Heater = 0,
    AdcChann_SleepDelay,
    AdcChann_TargetTemp,
    AdcChann_Max,
} AdcChannel_t;

void ADC_Config(void);
void AdcConvert(void);
uint16_t GetAdcResult(AdcChannel_t channel); 
