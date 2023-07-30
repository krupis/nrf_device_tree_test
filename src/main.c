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
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/lora.h>
#include <string.h>

#define LOG_LEVEL CONFIG_LOG_DBG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(lora);


#define TRANSMIT 1
#define RECEIVE 0


char data_tx[] = {"Hello"};

static struct gpio_callback button_callback_data;
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0});
K_SEM_DEFINE(pb_pushed, 0, 1);






#define DEFAULT_RADIO_NODE DT_ALIAS(lora0)
BUILD_ASSERT(DT_NODE_HAS_STATUS(DEFAULT_RADIO_NODE, okay),


void button_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printf("button clicked \n");
	k_sem_give(&pb_pushed);
}




void lora_recv_callback(const struct device *dev, uint8_t *data, uint16_t size, int16_t rssi, int8_t snr)
{
	// When lora_recv_async is cancelled, may be called with 0 bytes.
	if (size != 0) {
		printk("RECV %d bytes: ",size);
		for (uint16_t i = 0; i < size; i++)
			printk("0x%02x ",data[i]);
		printk("RSSI = %ddBm, SNR = %ddBm\n", rssi, snr);
	} 
}



int lora_configure(const struct device *dev, bool transmit)
{
	int ret;
	struct lora_modem_config config;

	config.frequency = 433000000;
	config.bandwidth = BW_125_KHZ;
	config.datarate = SF_10;
	config.preamble_len = 8;
	config.coding_rate = CR_4_5;
	config.tx_power = 4;
	config.iq_inverted = false;
	config.public_network = false;
	config.tx = transmit;

	ret = lora_config(dev, &config);
	if (ret < 0) {
		LOG_ERR("LoRa device configuration failed");
		return false;
	}

	return(true);
}



int main(void)
{




	//SHT40 Example
	/*
    const struct device *const dev = DEVICE_DT_GET_ONE(sensirion_sht4x);
	int rc;

	if (!device_is_ready(dev)) {
		printf("Device %s is not ready\n", dev->name);
		return 0;
	}
	else{
		printf("Sensirion sht40 device is ready \n");
	}


	while (true) {
		struct sensor_value temp, hum;

		rc = sensor_sample_fetch(dev);

		if (rc == 0) {
			rc = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP,
						&temp);
		}
		if (rc == 0) {
			rc = sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY,
						&hum);
		}
		if (rc != 0) {
			printf("SHT40: failed: %d\n", rc);
			break;
		}
		float temp_value = sensor_value_to_double(&temp);
		float humidity_value = sensor_value_to_double(&hum);
		printf("Temperature value = %.2f \n",temp_value);
		printf("Humidity value = %.2f \n",humidity_value);
		k_sleep(K_MSEC(2000));
	}
	*/
	//END OF SHT40 EXAMPLE



//LORA WITH BUTTON EXAMPLE
// 	const struct device *dev_lora;
// 	int ret, bytes;

// // Setup SW1 Momentary Push Button:
// 	if (!device_is_ready(button.port)) {
// 		printk("Error: button device %s is not ready\n", button.port->name);
// 		return;
// 	}

// 	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
// 	if (ret != 0) {
// 		printk("Error %d: failed to configure %s pin %d\n", ret, button.port->name, button.pin);
// 		return;
// 	}

// 	ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
// 	if (ret != 0) {
// 		printk("Error %d: failed to configure interrupt on %s pin %d\n", ret, button.port->name, button.pin);
// 		return;
// 	}

// 	gpio_init_callback(&button_callback_data, button_callback, BIT(button.pin));
// 	gpio_add_callback(button.port, &button_callback_data);


// 	printk("LoRa Point to Point Communications Example\n");

// 	// Setup LoRa Radio Device:
// 	dev_lora = DEVICE_DT_GET(DT_ALIAS(lora0));
// 	if (!device_is_ready(dev_lora)) {
// 		printk("%s: device not ready", dev_lora->name);
// 		return;
// 	}

// 	if (lora_configure(dev_lora, RECEIVE)) {
// 		printk("LoRa Device Configured\n");
// 	} else {
// 		return;
// 	}


// 	while (1) {
// 		// Wait for SW1 to be pressed. Onced pressed, transmit some data

// 		if (k_sem_take(&pb_pushed, K_FOREVER) != 0) {
// 			printk("Error taking sem\n");
// 		} 

// 		// Cancel reception
// 		ret = lora_recv_async(dev_lora, NULL);
// 		if (ret < 0) {
// 			//LOG_ERR("LoRa recv_async failed %d\n", ret);
// 			printf("LoRa recv_async failed %d\n", ret);
// 		} 

// 		// Reconfigure radio for transmit
// 		lora_configure(dev_lora, TRANSMIT);	

// 		// Transmit data
// 		ret = lora_send(dev_lora, data_tx, sizeof(data_tx));
// 		if (ret < 0) {
// 			//LOG_ERR("LoRa send failed");
// 			printf("Lora send failed \n");
// 		} else {
// 			bytes = sizeof(data_tx);
// 			printk("XMIT %d bytes: ", bytes);
// 			for (uint16_t i = 0; i < bytes; i++)
// 				printk("0x%02x ",data_tx[i]);
// 			printk("\n");
// 		}

// 		// Restart reception
// 		lora_configure(dev_lora, RECEIVE);	
// 		ret = lora_recv_async(dev_lora, lora_recv_callback);
// 		if (ret < 0) {
// 			//LOG_ERR("LoRa recv_async failed %d\n", ret);
// 			printf("LoRa recv_async failed %d\n", ret);
// 		} 
// 		k_sleep(K_MSEC(20));
// 	}

//     return 0;
//END OF LORA BUTTON EXAMPLE





const struct device *const lora_dev = DEVICE_DT_GET(DEFAULT_RADIO_NODE);
	struct lora_modem_config config;
	int ret;

	if (!device_is_ready(lora_dev)) {
		LOG_ERR("%s Device not ready", lora_dev->name);
		return 0;
	}

	config.frequency = 865100000;
	config.bandwidth = BW_125_KHZ;
	config.datarate = SF_10;
	config.preamble_len = 8;
	config.coding_rate = CR_4_5;
	config.iq_inverted = false;
	config.public_network = false;
	config.tx_power = 4;
	config.tx = true;

	ret = lora_config(lora_dev, &config);
	if (ret < 0) {
		LOG_ERR("LoRa config failed");
		return 0;
	}

	while (1) {
		ret = lora_send(lora_dev, data, MAX_DATA_LEN);
		if (ret < 0) {
			LOG_ERR("LoRa send failed");
			return 0;
		}

		LOG_INF("Data sent!");

		/* Send data at 1s interval */
		k_sleep(K_MSEC(1000));
	}
	return 0;



}



