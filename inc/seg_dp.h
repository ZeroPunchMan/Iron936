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
    SegDpChar_Max,
} SegDpChar_t;

void SegDp_Init(void);
void SegDp_SetChar(SegDpChar_t c);
void SegDp_SetDp(bool dp);

void SegDp_SetBitMask(uint8_t mask);

