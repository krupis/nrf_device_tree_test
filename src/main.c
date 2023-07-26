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

#include <string.h>


#define STACKSIZE 1024
#define PRIORITY 7

#define LED0_NODE DT_ALIAS(led0) // LED0_NODE = led0 defined in the .dts file
#define LED1_NODE DT_ALIAS(led1) // LED0_NODE = led0 defined in the .dts file
#define LED2_NODE DT_ALIAS(led2) // LED0_NODE = led0 defined in the .dts file
#define LED3_NODE DT_ALIAS(led3) // LED0_NODE = led0 defined in the .dts file


static void blink0(void);
static void blink1(void);
static void blink2(void);
static void blink3(void);

struct led {
	struct gpio_dt_spec spec;
	uint8_t num;
};

static const struct led led0 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0}),
	.num = 0,
};
static const struct led led1 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED1_NODE, gpios, {0}),
	.num = 1,
};
static const struct led led2 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED2_NODE, gpios, {0}),
	.num = 2,
};
static const struct led led3 = {
	.spec = GPIO_DT_SPEC_GET_OR(LED3_NODE, gpios, {0}),
	.num = 3,
};





int main(void)
{
    const struct device *const dev = DEVICE_DT_GET_ONE(sensirion_sht3xd);
	int rc;

	if (!device_is_ready(dev)) {
		printf("Device %s is not ready\n", dev->name);
		return 0;
	}
    return 0;
}




void blink0(void)
{
	blink(&led0, 100, 0);
}

void blink1(void)
{
	blink(&led1, 200, 0);
}

void blink2(void)
{
	blink(&led2, 400, 0);
}

void blink3(void)
{
	blink(&led3, 800, 0);
}


void blink(const struct led *led, uint32_t sleep_ms, uint32_t id)
{
	const struct gpio_dt_spec *spec = &led->spec;
	int cnt = 0;
	int ret;

	if (!device_is_ready(spec->port)) {
		printk("Error: %s device is not ready\n", spec->port->name);
		return;
	}

	ret = gpio_pin_configure_dt(spec, GPIO_OUTPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure pin %d (LED '%d')\n",
			ret, spec->pin, led->num);
		return;
	}

	while (1) {
		gpio_pin_set(spec->port, spec->pin, cnt % 2);
		k_msleep(sleep_ms);
		cnt++;
	}
}

K_THREAD_DEFINE(blink0_id, STACKSIZE, blink0, NULL, NULL, NULL,
		PRIORITY, 0, 0);

K_THREAD_DEFINE(blink1_id, STACKSIZE, blink1, NULL, NULL, NULL,
		PRIORITY, 0, 0);

K_THREAD_DEFINE(blink2_id, STACKSIZE, blink2, NULL, NULL, NULL,
		PRIORITY, 0, 0);

K_THREAD_DEFINE(blink3_id, STACKSIZE, blink3, NULL, NULL, NULL,
		PRIORITY, 0, 0);        