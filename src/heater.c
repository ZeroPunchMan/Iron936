#include "heater.h"
#include "adc.h"
#include "main.h"
#include "board.h"

typedef enum
{
    WS_Measure,
    WS_Heat,
} WorkStatus_t;

typedef struct
{
    WorkStatus_t workSta;
    bool onOff;
} WorkContext_t;

static WorkContext_t context = {
    .workSta = WS_Measure,
    .onOff = false,
};

void Heater_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin = SLEEP_PIN;
    GPIO_Init(SLEEP_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(SLEEP_PORT, SLEEP_PIN);

    GPIO_IOMUX_ChangePin(IOMUX_PIN11, IOMUX_PD5_SEL_PD5);
}

void Heater_Update(void)
{
    switch (context.workSta)
    {
    case WS_Measure:
        AdcConvert();
        if (context.onOff)
        {
        }
        break;
    case WS_Heat:
        break;
    }
}
