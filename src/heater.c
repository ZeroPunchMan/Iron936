#include "heater.h"
#include "adc.h"
#include "main.h"
#include "board.h"
#include "systime.h"
#include "tim.h"
#include "pid.h"
#include "helper.h"
#include "cl_log.h"
#include "seg_dp.h"

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
    uint8_t sleepDelay;
    bool sleep;
} WorkContext_t;

static WorkContext_t context = {
    .workSta = WS_Idle,
    .onOff = true,
    .startHeatTime = 0,
    .sleepDelay = 0,
    .sleep = false,
};

static PIDController pid = {
    .Kp = 80.0f,
    .Ki = 1.5f,
    .Kd = -1.0f,

    .limMinInt = -300.0f,
    .limMaxInt = 300.0f,

    .limMin = 0.0f,
    .limMax = 1000.0f,

    .T = 0.1f,
};

static void ToHeat(uint16_t pwmDuty);
static void ToMeasure(void);

void Heater_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
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
    context.workSta = WS_Idle;
    SetPwmDuty(PwmChan_Heater, 0);
}

static void IdleProc(void)
{
    if (context.onOff)
        ToMeasure();
}

static void ToMeasure(void)
{
    SetPwmDuty(PwmChan_Heater, 0);
    DelayOnSysTime(15);
    context.workSta = WS_Measure;
    uint16_t sensorAdc = 0;
    uint16_t tarTempAdc = 0;
    uint16_t sleepAdc = 0;
    for (int i = 0; i < 10; i++)
    {
        AdcConvert();
        sensorAdc += GetAdcResult(AdcChann_Heater);
        tarTempAdc += GetAdcResult(AdcChann_TargetTemp);
        sleepAdc += GetAdcResult(AdcChann_SleepDelay);
        // DelayOnSysTime(1);
    }
    sensorAdc /= 10;
    tarTempAdc /= 10;
    sleepAdc /= 10;

    static uint32_t lastMeasureTime = 0;
    uint32_t span = SysTimeSpan(lastMeasureTime);
    lastMeasureTime = GetSysTime();
    pid.T = span / 1000.0f;

    // CL_LOG_LINE("tar: %d, sensor: %d", tarTempAdc, sensorAdc);

    uint8_t sleep = GetSleepDelay(sleepAdc);
    if (sleep != context.sleepDelay)
    {
        context.sleepDelay = sleep;
        SegDp_SetSleepDelay(sleep);
        CL_LOG_LINE("sleep delay: %d", context.sleepDelay);
    }

    if (context.onOff)
    {
        // 传感器温度
        uint16_t sensorTemp = GetSensorTemp(sensorAdc);
        // 目标温度
        uint16_t tarTemp = 200;
        if (!context.sleep)
            tarTemp = GetTargetTemp(tarTempAdc);

        if (sensorTemp >= 0xfff0)
        { // todo error
            CL_LOG_LINE("sensor error");
            ToHeat(0);
        }
        else
        {
            SegDp_SetTarTemp(tarTemp);
            PIDController_Update(&pid, tarTemp, sensorTemp);
            ToHeat(pid.out);
        }
        // CL_LOG_LINE("%d\t%d\t%d\t%d\t%d", tarTempAdc, sensorAdc, tarTemp, sensorTemp, (int)pid.out);
    }
    else
    {
        ToIdle();
    }
}

static void ToHeat(uint16_t pwmDuty)
{
    context.workSta = WS_Heat;
    // SetPwmDuty(PwmChan_Heater, pwmDuty);
    SetPwmDuty(PwmChan_Heater, 0); // todo
    context.startHeatTime = GetSysTime();
}

static void HeatProc(void)
{
    if (SysTimeSpan(context.startHeatTime) >= 100)
    {
        ToMeasure();
    }
}

static inline bool HandleIdleCheck(void)
{
    static uint32_t levelChangeTime = 0;
    static uint8_t lastLevel = 0;
    uint8_t pinLvl = GPIO_ReadInputDataBit(SLEEP_PORT, SLEEP_PIN);

    if (pinLvl != lastLevel)
        levelChangeTime = GetSysTime();

    lastLevel = pinLvl;

    if (SysTimeSpan(levelChangeTime) > (context.sleepDelay * SYSTIME_SECOND(60)))
        return true;

    return false;
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

    bool sleep = HandleIdleCheck();

    if (context.sleep != sleep)
    {
        CL_LOG_LINE("sleep: %d", sleep);
        context.sleep = sleep;
    }
}
