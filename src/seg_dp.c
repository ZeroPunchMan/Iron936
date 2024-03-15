#include "seg_dp.h"
#include "board.h"
#include "cl_log.h"
#include "systime.h"

typedef struct
{
    GPIO_TypeDef *port;
    uint32_t pin;
} SegPin_t;

static const SegPin_t segSelPins[3] = {
    [0] = {.port = SEG_SEL_1_PORT, .pin = SEG_SEL_1_PIN},
    [1] = {.port = SEG_SEL_2_PORT, .pin = SEG_SEL_2_PIN},
    [2] = {.port = SEG_SEL_3_PORT, .pin = SEG_SEL_3_PIN},
};

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

#define SEG_MASK_F (1 << 0)
#define SEG_MASK_E (1 << 1)
#define SEG_MASK_D (1 << 2)
#define SEG_MASK_C (1 << 3)
#define SEG_MASK_B (1 << 4)
#define SEG_MASK_A (1 << 5)
#define SEG_MASK_G (1 << 6)

static const uint8_t segCharTable[SegDpChar_Max] = {
    SEG_MASK_F | SEG_MASK_E | SEG_MASK_D | SEG_MASK_C | SEG_MASK_B | SEG_MASK_A,              // 0
    SEG_MASK_C | SEG_MASK_B,                                                                  // 1
    SEG_MASK_E | SEG_MASK_D | SEG_MASK_B | SEG_MASK_A | SEG_MASK_G,                           // 2
    SEG_MASK_D | SEG_MASK_C | SEG_MASK_B | SEG_MASK_A | SEG_MASK_G,                           // 3
    SEG_MASK_F | SEG_MASK_C | SEG_MASK_B | SEG_MASK_G,                                        // 4
    SEG_MASK_F | SEG_MASK_D | SEG_MASK_C | SEG_MASK_A | SEG_MASK_G,                           // 5
    SEG_MASK_F | SEG_MASK_E | SEG_MASK_D | SEG_MASK_C | SEG_MASK_A | SEG_MASK_G,              // 6
    SEG_MASK_A | SEG_MASK_B | SEG_MASK_C,                                                     // 7
    SEG_MASK_F | SEG_MASK_E | SEG_MASK_D | SEG_MASK_C | SEG_MASK_B | SEG_MASK_A | SEG_MASK_G, // 8
    SEG_MASK_F | SEG_MASK_D | SEG_MASK_C | SEG_MASK_B | SEG_MASK_A | SEG_MASK_G,              // 9
    0,                                                                                        // off
};

static void SegDp_SetChar(SegDpChar_t c);

static inline void TurnOnSeg(uint8_t seg)
{
    GPIO_ResetBits(segPins[seg].port, segPins[seg].pin);
}

static inline void TurnOffSeg(uint8_t seg)
{
    GPIO_SetBits(segPins[seg].port, segPins[seg].pin);
}

static inline void SelectSeg(uint8_t offset)
{
    switch (offset)
    {
    case 0:
        GPIO_ResetBits(segSelPins[0].port, segSelPins[0].pin);
        GPIO_SetBits(segSelPins[1].port, segSelPins[1].pin);
        GPIO_SetBits(segSelPins[2].port, segSelPins[2].pin);
        break;
    case 1:
        GPIO_SetBits(segSelPins[0].port, segSelPins[0].pin);
        GPIO_ResetBits(segSelPins[1].port, segSelPins[1].pin);
        GPIO_SetBits(segSelPins[2].port, segSelPins[2].pin);
        break;
    case 2:
        GPIO_SetBits(segSelPins[0].port, segSelPins[0].pin);
        GPIO_SetBits(segSelPins[1].port, segSelPins[1].pin);
        GPIO_ResetBits(segSelPins[2].port, segSelPins[2].pin);
        break;
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

// void SegDp_SetBitMask(uint8_t mask)
// {
//     for (int i = 0; i < 8; i++)
//     {
//         if (mask & (1 << i))
//             TurnOnSeg(i);
//         else
//             TurnOffSeg(i);
//     }
// }

void SegDp_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    for (int i = 0; i < CL_ARRAY_LENGTH(segPins); i++)
    {
        GPIO_InitStructure.GPIO_Pin = segPins[i].pin;
        GPIO_Init(segPins[i].port, &GPIO_InitStructure);

        GPIO_ResetBits(segPins[i].port, segPins[i].pin);
    }

    for (int i = 0; i < CL_ARRAY_LENGTH(segSelPins); i++)
    {
        GPIO_InitStructure.GPIO_Pin = segSelPins[i].pin;
        GPIO_Init(segSelPins[i].port, &GPIO_InitStructure);

        GPIO_ResetBits(segSelPins[i].port, segSelPins[i].pin);
    }
    GPIO_IOMUX_ChangePin(IOMUX_PIN11, IOMUX_PB5_SEL_PB5);
}

static uint8_t disNumber[3] = {0};
void SegDp_Update(void)
{
    static uint8_t count = 0;
    count++;
    if (count >= 3)
        count = 0;

    SegDp_SetChar(SegDpChar_Off);
    SelectSeg(count);
    SegDp_SetChar((SegDpChar_t)disNumber[count]);
}

static uint32_t setDelayTime = 0;
static uint8_t sleepDelay = 1;
static uint16_t targetTemp = 120;
void SegDp_Process(void)
{
    if (SysTimeSpan(setDelayTime) < 3000 && setDelayTime != 0)
    {
        disNumber[0] = SegDpChar_Off;
        disNumber[1] = (sleepDelay % 100) / 10;
        disNumber[2] = (sleepDelay % 10);
    }
    else
    {
        disNumber[0] = (targetTemp % 1000) / 100;
        disNumber[1] = (targetTemp % 100) / 10;
        disNumber[2] = (targetTemp % 10);
    }
}

void SegDp_SetSleepDelay(uint8_t delay)
{
    sleepDelay = delay;

    static bool firstTime = true;
    if (!firstTime)
    {
        setDelayTime = GetSysTime();
    }
    else
    {
        firstTime = false;
    }
}

void SegDp_SetTarTemp(uint16_t temp)
{
    targetTemp = temp;
}
