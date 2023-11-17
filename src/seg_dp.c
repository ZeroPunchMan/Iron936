#include "seg_dp.h"
#include "board.h"

typedef struct
{
    GPIO_TypeDef *port;
    uint32_t pin;
} SegPin_t;

static const SegPin_t segPins[7] = {
    [0] = {.port = SEG_F_PORT, .pin = SEG_F_PIN},
    [1] = {.port = SEG_E_PORT, .pin = SEG_E_PIN},
    [2] = {.port = SEG_D_PORT, .pin = SEG_D_PIN},
    [3] = {.port = SEG_C_PORT, .pin = SEG_C_PIN},
    [4] = {.port = SEG_B_PORT, .pin = SEG_B_PIN},
    [5] = {.port = SEG_A_PORT, .pin = SEG_A_PIN},
    [6] = {.port = SEG_G_PORT, .pin = SEG_G_PIN},
    // [7] = {.port = SEG_DP_PORT, .pin = SEG_DP_PIN},
};

#define SEG_MASK(x) (1 << x)

static const uint8_t segCharTable[SegDpChar_Max] = {
    SEG_MASK(0) | SEG_MASK(1) | SEG_MASK(2) | SEG_MASK(3) | SEG_MASK(4) | SEG_MASK(5),
    SEG_MASK(3) | SEG_MASK(4),
    SEG_MASK(1) | SEG_MASK(2) | SEG_MASK(4) | SEG_MASK(5) | SEG_MASK(6),
    SEG_MASK(2) | SEG_MASK(3) | SEG_MASK(4) | SEG_MASK(5) | SEG_MASK(6),
    SEG_MASK(0) | SEG_MASK(3) | SEG_MASK(4) | SEG_MASK(6),
    SEG_MASK(0) | SEG_MASK(2) | SEG_MASK(3) | SEG_MASK(5) | SEG_MASK(6),
};

static inline void TurnOnSeg(uint8_t seg)
{
    GPIO_SetBits(segPins[seg].port, segPins[seg].pin);
}

static inline void TurnOffSeg(uint8_t seg)
{
    GPIO_ResetBits(segPins[seg].port, segPins[seg].pin);
}

void SegDp_Init(void)
{
    GPIO_IOMUX_ChangePin(IOMUX_PIN5, IOMUX_PB5_SEL_PB5);

    GPIO_InitTypeDef GPIO_InitStructure;
    for (int i = 0; i < CL_ARRAY_LENGTH(segPins); i++)
    {
        GPIO_InitStructure.GPIO_Pin = segPins[i].pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(segPins[i].port, &GPIO_InitStructure);

        GPIO_ResetBits(segPins[i].port, segPins[i].pin);
    }
    
}

void SegDp_SetChar(SegDpChar_t c)
{
    uint8_t mask = segCharTable[c];
    for (int i = 0; i < 7; i++)
    {
        if (mask & (1 << i))
            TurnOnSeg(i);
        else
            TurnOffSeg(i);
    }
}

void SegDp_SetDp(bool dp)
{
    if (dp)
        TurnOnSeg(7);
    else
        TurnOffSeg(7);
}

void SegDp_SetBitMask(uint8_t mask)
{
    for (int i = 0; i < 8; i++)
    {
        if (mask & (1 << i))
            TurnOnSeg(i);
        else
            TurnOffSeg(i);
    }
}
