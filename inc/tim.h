#pragma once

#include "cl_common.h"

void TIM_Config(void);

typedef enum
{
    PwmChan_Heater = 0,
    PwmChan_Max,
} PwmChannel_t;

void SetPwmDuty(PwmChannel_t chann, uint16_t duty); //duty 0~1000
uint16_t GetPwmDuty(PwmChannel_t chann);

