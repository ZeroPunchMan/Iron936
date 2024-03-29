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
    WS_Measure,
    WS_Heat,
} WorkStatus_t;

typedef struct
{
    WorkStatus_t workSta;
    uint32_t startHeatTime;
    uint8_t sleepDelay;
    uint32_t lastActionTime;
    uint16_t targetTemp;
    bool limitCurrent;
    uint32_t limitTime;
} WorkContext_t;

static WorkContext_t context = {
    .workSta = WS_Measure,
    .startHeatTime = 0,
    .sleepDelay = 0,
    .lastActionTime = 0,
    .targetTemp = 200,
    .limitCurrent = false,
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
    ToHeat(0);
}

static void ToMeasure(void)
{
    context.workSta = WS_Measure;

    //*****关PWM
    SetPwmDuty(PwmChan_Heater, 0);

    DelayOnSysTime(15);

    //****测ADC
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

    //*****计算测量间隔
    static uint32_t lastMeasureTime = 0;
    uint32_t span = SysTimeSpan(lastMeasureTime);
    lastMeasureTime = GetSysTime();
    pid.T = span / 1000.0f;

    // CL_LOG_LINE("tar: %d, sensor: %d", tarTempAdc, sensorAdc);

    //******计算休眠时间
    uint8_t sleep = GetSleepDelay(sleepAdc);
    if (sleep != context.sleepDelay)
    {
        context.sleepDelay = sleep;
        SegDp_SetSleepDelay(sleep);
        CL_LOG_LINE("sleep delay: %d", context.sleepDelay);
    }

    // 目标温度
    uint16_t tarTemp = GetTargetTemp(tarTempAdc);
    if (tarTemp != context.targetTemp)
    {
        context.targetTemp = tarTemp;
        context.lastActionTime = GetSysTime();
    }

    if (SysTimeSpan(context.lastActionTime) < (context.sleepDelay * SYSTIME_SECOND(60)))
    {
    }
    else if (SysTimeSpan(context.lastActionTime) < (context.sleepDelay * 2 * SYSTIME_SECOND(60)))
    {
        tarTemp = 120;
    }
    else
    {
        tarTemp = 0;
    }

    SegDp_SetTarTemp(tarTemp);
    if (tarTemp > 0)
    {
        // 传感器温度
        uint16_t sensorTemp = GetSensorTemp(sensorAdc);
        if (sensorTemp >= 0xfff0)
        { // error
            CL_LOG_LINE("sensor error");
            ToHeat(0);
        }
        else
        {
            PIDController_Update(&pid, tarTemp, sensorTemp);
            if (context.limitCurrent)
            {
                uint16_t out = pid.out;
                ToHeat(CL_MIN(out, PRT_PWM));
                CL_LOG_LINE("limit: %d\t%d\t%d", tarTemp, sensorTemp, CL_MIN(out, PRT_PWM));
            }
            else
            {
                ToHeat(pid.out);
                CL_LOG_LINE("full: %d\t%d\t%d", tarTemp, sensorTemp, (int)pid.out);
            }
        }
        // CL_LOG_LINE("%d\t%d\t%d\t%d\t%d", tarTempAdc, sensorAdc, tarTemp, sensorTemp, (int)pid.out);
    }
    else
    {
        ToHeat(0);
        CL_LOG_LINE("target: %d", tarTemp);
    }
}

static void ToHeat(uint16_t pwmDuty)
{
    context.workSta = WS_Heat;
    SetPwmDuty(PwmChan_Heater, pwmDuty);
    // SetPwmDuty(PwmChan_Heater, 0); // todo
    context.startHeatTime = GetSysTime();
}

static void HeatProc(void)
{
    if (SysTimeSpan(context.startHeatTime) >= 100)
    {
        ToMeasure();
    }
}

static inline void HandleIdleCheck(void)
{
    static uint32_t lastSecTime = 0;
    static uint16_t lvlCount = 0;

    if (SysTimeSpan(lastSecTime) < 1000)
    {
        static uint8_t lastLevel = 0;
        uint8_t pinLvl = GPIO_ReadInputDataBit(SLEEP_PORT, SLEEP_PIN);
        if (pinLvl != lastLevel)
            lvlCount++;

        lastLevel = pinLvl;
    }
    else
    {
        lastSecTime = GetSysTime();

        if (lvlCount >= 4)
            context.lastActionTime = GetSysTime();

        lvlCount = 0;
    }
}

void Heater_Process(void)
{
    switch (context.workSta)
    {
    case WS_Measure:
        break;
    case WS_Heat:
        HeatProc();
        break;
    }

    HandleIdleCheck();

    if (GetSysTime() > SYSTIME_SECOND(1))
    {
        if (GetAdcResult(AdcChann_Voltage) < PRT_ADC) // 检测电压,是否限制电流
        {
            context.limitCurrent = true;
            context.limitTime = GetSysTime();
        }

        if(context.limitCurrent && SysTimeSpan(context.limitTime) > SYSTIME_SECOND(60))
        {
            context.limitCurrent = false;
        }
    }
}
