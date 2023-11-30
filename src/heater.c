#include "heater.h"
#include "adc.h"
#include "main.h"
#include "board.h"
#include "systime.h"
#include "tim.h"
#include "pid.h"

typedef enum
{
    WS_Idle,
    WS_Measure,
    WS_Heat,
} WorkStatus_t;

typedef struct
{
    WorkStatus_t workSta;
    bool onOff;
    uint32_t startHeatTime;
} WorkContext_t;

static WorkContext_t context = {
    .workSta = WS_Idle,
    .onOff = false,
    .startHeatTime = 0,
};

static PIDController pid = {
    .Kp = 1.0f,
    .Ki = 1.0f,
    .Kd =1.0f,

    .limMinInt = -10.0f,
    .limMaxInt = 10.0f,

    .limMin = 0.0f,
    .limMax = 100.0f,

    .T = 0.1f,
};

static void ToHeat(uint8_t pwmDuty);
static void ToMeasure(void);

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

    PIDController_Init(&pid);
}

static void ToIdle(void)
{
    SetPwmDuty(PwmChan_Heater, 0);
}

static void IdleProc(void)
{
    if (context.onOff)
    {
        ToMeasure();
    }
}

static void MeasureDelay(int x)
{
    while (x--)
    {
        for (int i = 0; i < 1000; i++)
            ;
    }
}

static inline uint16_t GetTargetTemp(uint16_t adc)
{
    return adc; //todo
}

static inline uint16_t GetSensorTemp(uint16_t adc)
{
    return adc; //todo
}

static void ToMeasure(void)
{
    SetPwmDuty(PwmChan_Heater, 0);
    MeasureDelay(100);

    uint16_t sensorAdc = 0;
    uint16_t tarTempAdc = 0;
    for (int i = 0; i < 10; i++)
    {
        AdcConvert();
        sensorAdc += GetAdcResult(AdcChann_Heater);
        tarTempAdc += GetAdcResult(AdcChann_TargetTemp);
    }
    sensorAdc /= 10;
    tarTempAdc /= 10;

    if (context.onOff)
    {
        // pid
        PIDController_Update(&pid, GetTargetTemp(tarTempAdc), GetSensorTemp(sensorAdc));
        ToHeat(pid.out);
    }
    else
    {
        ToIdle();
    }
}

static void ToHeat(uint8_t pwmDuty)
{
    SetPwmDuty(PwmChan_Heater, pwmDuty);
    context.startHeatTime = GetSysTime();
}

static void HeatProc(void)
{
    if (SysTimeSpan(context.startHeatTime) >= 100)
    {
        ToMeasure();
    }
}

void Heater_Process(void)
{
    switch (context.workSta)
    {
    case WS_Idle:
        IdleProc();
        break;
    case WS_Measure:
        break;
    case WS_Heat:
        HeatProc();
        break;
    }
}
