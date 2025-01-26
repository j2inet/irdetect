#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "pico/binary_info.h"
#include "pico/cyw43_arch.h"
#include <iostream>
#include "squarewave.pio.h"


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

const uint CARRIER_PIN = 9;

static const float pio_freq = 38000*2;


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

	//gpio_init(CARRIER_PIN);
	//gpio_set_dir(CARRIER_PIN, GPIO_OUT);
}

// Turn the LED on or off
void pico_set_led(bool led_on) {
	gpio_put(LED_PIN, led_on);
	gpio_put(IR_DETECTOR_ENABLE_PIN, true);
}

int main()
{
	PIO pio;
    uint sm;
    uint offset;

	setup_default_uart();
	stdio_init_all();
	pico_led_init();
	bool irDetected = false;
	
	std::cout  << std::endl << std::endl<< "Initialized" << std::endl;

	float div = (float)clock_get_hz(clk_sys) / pio_freq;
	std::cout << "To achieve a frequence of " << pio_freq << " Hz, will use a divisor of "<<div<<" for the clock " << std::endl;
	bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&squarewave_program, &pio, &sm, &offset, CARRIER_PIN, 1, true);
	std::cout << "Success:" << success << std::endl;
	//pio->sm[sm].clkdiv = (uint32_t) (2.5f * (1 << 16));
    hard_assert(success);
	squarewave_program_init(pio, sm, offset, CARRIER_PIN, div);
	
	std::cout << "Running main loop" << std::endl;

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
	pio_remove_program_and_unclaim_sm(&squarewave_program, pio, sm, offset);
	return 0;
}
