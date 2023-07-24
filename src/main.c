/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

#define LED0_NODE DT_ALIAS(led0) // LED0_NODE = led0 defined in the .dts file

int main(void)
{

    static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

    if (!device_is_ready(led0.port))
    {
        return;
    }
    int ret;

    ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
    if (ret < 0)
    {
        return;
    }

    printf("Hello World! %s\n", CONFIG_BOARD);
    k_sleep(K_MSEC(1000));
    printf("Bye World! %s\n", CONFIG_BOARD);

    while (1)
    {
        ret = gpio_pin_toggle_dt(&led0);
        if (ret < 0)
        {
            return;
        }
        k_msleep(1000);
    }

    return 0;
}
