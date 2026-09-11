#ifndef BME_TEMPERATURE_H
#define BME_TEMPERATURE_H

#include <stdint.h>

static inline int32_t bme_decode_raw(const uint8_t data[3])
{
    return ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
}

static inline double bme_compensate_temperature(
    int32_t raw, uint16_t t1, int16_t t2, int16_t t3)
{
    double var1 = (raw / 16384.0 - t1 / 1024.0) * t2;
    double difference = raw / 131072.0 - t1 / 8192.0;
    double var2 = difference * difference * t3;

    return (var1 + var2) / 5120.0;
}

#endif