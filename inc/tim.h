#pragma once

#include "cl_common.h"

void TIM_Config(void);

typedef enum
{
    PwmChan_Heater,
} PwmChannel_t;

void SetPwmDuty(PwmChannel_t chann, uint8_t duty);

