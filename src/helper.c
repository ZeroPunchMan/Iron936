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
        return 0xfffe;
    else if (adc > table[tabLen - 1].adc)
        return 0xffff;

    for (uint16_t i = 0; i < tabLen - 1; i++)
    {
        if (VALUE_BETWEEN(adc, table[i].adc, table[i + 1].adc))
        {
            float result = ((float)adc - table[i].adc) / ((float)table[i + 1].adc - table[i].adc) * ((float)table[i + 1].temp - table[i].temp);
            return result;
        }
    }

    return 0xfffd;
}

#define TAR_ADC_TO_R(adc) (adc * 1000UL / (4096UL - adc))

uint16_t GetTargetTemp(uint16_t adc)
{
    uint32_t r = TAR_ADC_TO_R(adc);
    CL_LOG_LINE("r: %d", r);
    if (r > 10000)
        return 120;
    else if (r < 5)
        return 480;
    else
        return 480 - r * (480.0f - 120) / 10000;
}

static const Adc2Temp_t sensorTempTable[] = {
    {0, 0},      // todo 室温
    {1450, 127}, // 3%
    {1580, 153}, // 5%
    {1870, 219}, // 10%
    {2085, 268}, // 15%
    {2257, 308}, // 20%
    {2397, 343}, // 25%
    {2524, 380}, // 32%
    {2555, 420}, // 40%  到上限了????
};

uint16_t GetSensorTemp(uint16_t adc)
{
    return TempInterpolate(sensorTempTable, CL_ARRAY_LENGTH(sensorTempTable), adc);
}
