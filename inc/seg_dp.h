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
    SegDpChar_Max,
} SegDpChar_t;

void SegDp_Init(void);

void SegDp_SetNumber(uint8_t num1, uint8_t num2, uint8_t num3);
// void SegDp_SetChar(SegDpChar_t c);
// void SegDp_SetDp(bool dp);

// void SegDp_SetBitMask(uint8_t mask);


void SegDp_Update(void);
