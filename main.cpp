#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/cyw43_arch.h"


const uint LED_DELAY_MS = 50;
#ifdef PICO_DEFAULT_LED_PIN
const uint LED_PIN = PICO_DEFAULT_LED_PIN;
#else
const uint LED_PIN = 15;
#endif
const uint IR_READ_PIN = 14;
const uint IR_DETECTOR_ENABLE_PIN = 13;

const uint TV_IR_ENABLE = 5;
const uint TV_IR_DETECT = 6;
const uint TV_IR_STATUS = 7;


// Initialize the GPIO for the LED
void pico_led_init(void) {
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	gpio_init(IR_READ_PIN);
	gpio_set_dir(IR_READ_PIN, GPIO_IN);

	gpio_init(IR_DETECTOR_ENABLE_PIN);
	gpio_set_dir(IR_DETECTOR_ENABLE_PIN, GPIO_OUT);

	gpio_init(TV_IR_ENABLE);
	gpio_set_dir(TV_IR_ENABLE, GPIO_OUT);
	gpio_put(TV_IR_ENABLE, true);

	gpio_init(TV_IR_DETECT);
	gpio_set_dir(TV_IR_DETECT, GPIO_IN);
	gpio_pull_up(TV_IR_DETECT);

	gpio_init(TV_IR_STATUS);
	gpio_set_dir(TV_IR_STATUS, GPIO_OUT);
}

// Turn the LED on or off
void pico_set_led(bool led_on) {
	gpio_put(LED_PIN, led_on);
	gpio_put(IR_DETECTOR_ENABLE_PIN, true);
}

int main()
{
	stdio_init_all();
	pico_led_init();
	bool irDetected = false;

	while(!irDetected)
	{
		irDetected = gpio_get(IR_READ_PIN);
		pico_set_led(true);
		sleep_ms(LED_DELAY_MS);
		pico_set_led(false);
		sleep_ms(LED_DELAY_MS);
	}

	gpio_put(TV_IR_ENABLE, true);

	while(true)
	{
		bool tv_status = gpio_get(TV_IR_DETECT);
		gpio_put(TV_IR_STATUS, tv_status);
		gpio_put(TV_IR_DETECT, true);

		bool p = gpio_get(IR_READ_PIN);
		gpio_put(LED_PIN, p);
		sleep_us(10);
	}
	return 0;
}
