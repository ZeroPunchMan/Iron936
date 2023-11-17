#pragma once

#include "cl_common.h"

void ADC_Config(void);
void OnAdcChannDone(uint8_t channel);
void StartAdcOneSeq(void);
void OnAdcSeqDone(void);
bool IsAdcConvDone(void);
uint16_t GetAdcResult(uint8_t channel);
