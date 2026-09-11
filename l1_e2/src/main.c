/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   2000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led5180)
#define BUTTON_NODE DT_ALIAS(sw0)


/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

int main(void)
{
	int ret;
	bool led_state = true;

	if (!gpio_is_ready_dt(&led) || !gpio_is_ready_dt(&button)) {
		return 0;
	}

	if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0) {
        return 0;
    }

    if (gpio_pin_configure_dt(&button, GPIO_INPUT) < 0) {
        return 0;
	}

    int last_reading = 0;
    int stable_state = 0;
    int64_t last_change = k_uptime_get();


	while (1) {
		int pressed = gpio_pin_get_dt(&button);

        if (pressed < 0) {
            return 0;
        }

        int64_t now = k_uptime_get();

        if (pressed != last_reading) {
            last_reading = pressed;
            last_change = now;
        }

        if (pressed != stable_state && now - last_change >= 30) {
            stable_state = pressed;

            if (stable_state == 1) {
                if (gpio_pin_toggle_dt(&led) < 0) {
                    return 0;
                }
            }
        }

        k_msleep(10);
	}
	return 0;
}
