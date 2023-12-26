#pragma once

#include "cl_common.h"

typedef enum
{
    SegDpChar_0 = 0,
    SegDpChar_1,
    SegDpChar_2,
    SegDpChar_3,
    SegDpChar_4,
    SegDpChar_5,
    SegDpChar_6,
    SegDpChar_7,
    SegDpChar_8,
    SegDpChar_9,
    SegDpChar_Off,
    SegDpChar_Max,
} SegDpChar_t;

void SegDp_Init(void);
void SegDp_Process(void);

void SegDp_SetSleepDelay(uint8_t delay);
void SegDp_SetTarTemp(uint16_t temp);

void SegDp_Update(void);
