#pragma once

#include "cl_common.h"

void ADC_Config(void);
void AdcConvert(void);
uint16_t GetAdcResult(uint8_t channel);
