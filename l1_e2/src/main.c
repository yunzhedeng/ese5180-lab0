#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <errno.h>

static const struct i2c_dt_spec bme =
    I2C_DT_SPEC_GET(DT_NODELABEL(bme5180));

static uint16_t dig_T1;
static int16_t dig_T2;
static int16_t dig_T3;

static int wait_ready(void)
{
    for (int i = 0; i < 50; i++) {
        uint8_t status;
        int ret = i2c_reg_read_byte_dt(&bme, 0xF3, &status);

        if (ret < 0) {
            return ret;
        }

        if ((status & 0x09) == 0) {
            return 0;
        }

        k_msleep(2);
    }

    return -ETIMEDOUT;
}

static double compensate_temperature(int32_t raw)
{
    double var1 = (raw / 16384.0 - dig_T1 / 1024.0) * dig_T2;

    double difference = raw / 131072.0 - dig_T1 / 8192.0;

    double var2 = difference * difference * dig_T3;

    return (var1 + var2) / 5120.0;
}

int main(void)
{
    int ret;
    uint8_t chip_id;
    uint8_t calibration[6];

    if (!i2c_is_ready_dt(&bme)) {
        printk("I2C controller not ready\n");
        return 0;
    }

    k_msleep(10);

    ret = i2c_reg_read_byte_dt(&bme, 0xD0, &chip_id);
    if (ret < 0) {
        printk("Chip ID read failed: %d\n", ret);
        return 0;
    }

    if (chip_id != 0x60) {
        printk("Unexpected chip ID: 0x%02x\n",
               (unsigned int)chip_id);
        return 0;
    }

    printk("BME280 connected!\n");

    ret = i2c_reg_write_byte_dt(&bme, 0xE0, 0xB6);
    if (ret < 0) {
        printk("Sensor reset failed: %d\n", ret);
        return 0;
    }

    k_msleep(10);

    ret = wait_ready();
    if (ret < 0) {
        printk("Sensor startup failed: %d\n", ret);
        return 0;
    }

    ret = i2c_burst_read_dt(&bme, 0x88,
                           calibration, sizeof(calibration));
    if (ret < 0) {
        printk("Calibration read failed: %d\n", ret);
        return 0;
    }

    dig_T1 = sys_get_le16(&calibration[0]);
    dig_T2 = (int16_t)sys_get_le16(&calibration[2]);
    dig_T3 = (int16_t)sys_get_le16(&calibration[4]);

    printk("Calibration: T1=%u, T2=%d, T3=%d\n",
           (unsigned int)dig_T1, (int)dig_T2, (int)dig_T3);

    while (1) {
        ret = i2c_reg_write_byte_dt(&bme, 0xF4, 0x21);
        if (ret < 0) {
            printk("Measurement start failed: %d\n", ret);
            k_msleep(2000);
            continue;
        }

        k_msleep(10);

        ret = wait_ready();
        if (ret < 0) {
            printk("Measurement wait failed: %d\n", ret);
            k_msleep(2000);
            continue;
        }

        uint8_t data[3];

        ret = i2c_burst_read_dt(&bme, 0xFA, data, sizeof(data));
        if (ret < 0) {
            printk("Temperature read failed: %d\n", ret);
            k_msleep(2000);
            continue;
        }

        int32_t raw = ((int32_t)data[0] << 12)
                    | ((int32_t)data[1] << 4)
                    | (data[2] >> 4);

        if (raw == 0x80000) {
            printk("Temperature sample unavailable\n");
            k_msleep(2000);
            continue;
        }

        double temperature = compensate_temperature(raw);

        printk("Raw: %ld | Temperature: %.2f C\n",
               (long)raw, temperature);

        k_msleep(2000);
    }

    return 0;
}