#include <zephyr/ztest.h>
#include <zephyr/devicetree.h>
#include <errno.h>
#include <string.h>

#include "bme_temperature.h"

#define BME_NODE DT_NODELABEL(bme5180)

static int mock_read(uint8_t reg, uint8_t *data, size_t len)
{
    if (reg == 0xFA && len == 3) {
        const uint8_t sample[] = {0x7E, 0xED, 0x00};
        memcpy(data, sample, sizeof(sample));
        return 0;
    }

    return -EINVAL;
}

ZTEST(bme280_suite, test_devicetree)
{
    zassert_true(DT_NODE_EXISTS(BME_NODE), "BME280 node is missing");

    zassert_true(DT_NODE_HAS_STATUS(BME_NODE, okay), "BME280 node is disabled");

    zassert_equal(DT_REG_ADDR(BME_NODE), 0x77, "Incorrect I2C address");

    zassert_true(DT_ON_BUS(BME_NODE, i2c), "BME280 must be on an I2C bus");

    zassert_true(DT_NODE_HAS_STATUS(DT_BUS(BME_NODE), okay), "I2C bus is disabled");

    zassert_equal(DT_PROP(DT_BUS(BME_NODE), clock_frequency), 100000, "Expected 100 kHz");
}

ZTEST(bme280_suite, test_raw_decode)
{
    uint8_t data[3];

    zassert_equal(mock_read(0xFA, data, sizeof(data)), 0, "Mock read failed");

    zassert_equal(bme_decode_raw(data), 519888, "Incorrect 20-bit raw value");

    data[2] = 0x0F;

    zassert_equal(bme_decode_raw(data), 519888, "Unused bits affected the raw value");
}

ZTEST(bme280_suite, test_temperature_compensation)
{
    uint8_t data[3];

    zassert_equal(mock_read(0xFA, data, sizeof(data)), 0, "Mock read failed");

    int32_t raw = bme_decode_raw(data);

    double temperature = bme_compensate_temperature(raw, 27504, 26435, -1000);

    zassert_true(temperature > 25.07 && temperature < 25.09, "Expected approximately 25.08 C");
}

ZTEST_SUITE(bme280_suite, NULL, NULL, NULL, NULL, NULL);