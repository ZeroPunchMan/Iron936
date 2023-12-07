#include "helper.h"

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
            float result = ((float)adc -table[i].adc) / ((float)table[i + 1].adc- table[i].adc) * ((float)table[i + 1].temp- table[i].temp);
            return result; 
        }
    }

    return 0xfffd;
}

static const Adc2Temp_t tarTempTable[] = {
    {0, 0},
    {1000, 100},
    {2000, 200},
    {3000, 300},
    {4000, 400},
    {5000, 500},
};

uint16_t GetTargetTemp(uint16_t adc)
{
    return TempInterpolate(tarTempTable, CL_ARRAY_LENGTH(tarTempTable), adc);
}

static const Adc2Temp_t sensorTempTable[] = {
    {0, 0},
    {1000, 100},
    {2000, 200},
    {3000, 300},
    {4000, 400},
    {5000, 500},
};

uint16_t GetSensorTemp(uint16_t adc)
{
    return TempInterpolate(sensorTempTable, CL_ARRAY_LENGTH(sensorTempTable), adc);
}
