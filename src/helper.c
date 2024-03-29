#include "helper.h"
#include "cl_log.h"

typedef struct
{
    uint16_t adc;
    uint16_t temp;
} Adc2Temp_t;

#define VALUE_BETWEEN(n, a, b) ((n >= a && n <= b) || (n >= b && n <= a))

static uint16_t TempInterpolate(const Adc2Temp_t *table, uint16_t tabLen, uint16_t adc)
{
    if (adc < table[0].adc)
        return table[0].temp;
    else if (adc > table[tabLen - 1].adc)
        return table[tabLen - 1].temp;

    for (uint16_t i = 0; i < tabLen - 1; i++)
    {
        if (VALUE_BETWEEN(adc, table[i].adc, table[i + 1].adc))
        {
            float result = ((float)adc - table[i].adc) / ((float)table[i + 1].adc - table[i].adc) * ((float)table[i + 1].temp - table[i].temp);
            result += table[i].temp;
            return result;
        }
    }

    return 0xfffd; // error
}

#define TAR_ADC_TO_R(adc) (adc * 1000UL / (4096UL - adc))

uint16_t GetTargetTemp(uint16_t adc)
{
    static uint16_t tarTemp = 0;
    uint16_t result;
    uint32_t r = TAR_ADC_TO_R(adc);
    // CL_LOG_LINE("r: %d", r);
    if (r > 10000)
    {
        result = 120;
    }
    else if (r < 5)
    {
        result = 480;
    }
    else
    {
        result = 480 - r * (480 - 120) / 10000;
    }

    if (result >= (tarTemp + 5) || (result + 5) <= tarTemp)
        tarTemp = result;

    uint16_t rem = tarTemp % 5;
    if (rem >= 3)
        tarTemp += 5 - rem;
    else
        tarTemp -= rem;

    return tarTemp;
}

#define SLP_ADC_TO_R(adc) (adc * 1000UL / (4096UL - adc))
uint8_t GetSleepDelay(uint16_t adc)
{
    static uint8_t sleep = 0;
    uint8_t result;
    uint32_t r = TAR_ADC_TO_R(adc);
    if (r > 5000)
    {
        result = 2;
    }
    else if (r < 5)
    {
        result = 120;
    }
    else
    {
        result = 120 - r * (120 - 2) / 5000;
    }

    if (result >= (sleep + 2) || (result + 2) <= sleep)
        sleep = result;

    return sleep / 2;
}

static const Adc2Temp_t sensorTempTable[] = {
    {788, 15},   // 0% 室温
    {1124, 85}, // 3%
    {1380, 150}, // 8%
    {1634, 220}, // 15%
    {1860, 285}, // 24%
    {2106, 350}, // 35%
    {2298, 400}, // 48%
    {2454, 442}, // 60%
    {2550, 482}, // 65%
};

uint16_t GetSensorTemp(uint16_t adc)
{
    return TempInterpolate(sensorTempTable, CL_ARRAY_LENGTH(sensorTempTable), adc);
}
