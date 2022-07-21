#include "button.h"

#include "adc.h"
#include "config.h"
#include "defs.h"
#include "power.h"
#include "led.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <util/delay.h>

/**
 * @brief Initialisation function for the mcu on / off button and INT0 interrupt.
 * 
 */
void button_init()
{
	DDRB &= ~(1 << 6);	// configre PB6 as an input
	
	enable_on_interrupt();
	// level interrupt INT0 (low level)
    MCUCR &= ~((1 << ISC01) | (1 << ISC00));
}

/**
 * @brief Function to be called when the interupt awakes the mcu from sleeping.
 * It will powerup all mcu modules + bluetooth module and amp.
 * 
 */
void wakey_wakey()
{
	// CLEAR_BIT(CLKPR, CLKPS3);
	// SET_BIT(CLKPR, CLKPCE);

	// power up modules on mcu
	power_adc_enable();
	power_timer1_enable();
	power_timer0_enable();
	power_usi_enable();

	// Digital Input (buffer) Disable Registers
	// DIDR0 &= ~(MASK(ADC6D) | MASK(ADC5D) | MASK(ADC4D) | MASK(ADC3D) | MASK(AREFD) | MASK(ADC2D) | MASK(ADC1D) | MASK(ADC0D));
	// DIDR1 &= ~(MASK(ADC10D) | MASK(ADC9D) | MASK(ADC8D) | MASK(ADC7D));

	adc_init();	// assuming adc is still configured, make adc enable and disable functions
	power_enable_ble();
}

/**
 * @brief Function to be called when the off command is given.
 * Places the mcu and oher components in low power mode.
 * 
 */
void nap_time()
{
	// set lower clock speed
	// SET_BIT(CLKPR, CLKPS3);
	// SET_BIT(CLKPR, CLKPCE);

	power_disable_ble();
	led_set_full(0, 0, 0, 0, 0, 0);

	// power down modules on mcu
	power_adc_disable();
	power_timer1_disable();
	power_timer0_disable();
	power_usi_disable();
	// DDRA = 0;
	// DDRB = 0;


	// Digital Input (buffer) Disable Registers
	// DIDR0 |= (MASK(ADC6D) | MASK(ADC5D) | MASK(ADC4D) | MASK(ADC3D) | MASK(AREFD) | MASK(ADC2D) | MASK(ADC1D) | MASK(ADC0D));
	// DIDR1 |= (MASK(ADC10D) | MASK(ADC9D) | MASK(ADC8D) | MASK(ADC7D));
}

/**
 * @brief Call to place the MCU itself in a low power mode.
 * Be sure to configure an interupt that can wake the MCU from the "Power Down"
 * sleep mode.
 * 
 */
void enter_deepsleep()
{
	cli();
	enable_on_interrupt();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_bod_disable();
	sleep_enable();
	nap_time();
	sei();
	sleep_cpu();
}


/**
 * @brief Function to be used in the main program loop.
 * Checks if on / off button is pressed by polling it.
 * If pressed configure the MCU to go in sleep mode.
 * 
 */
void check_if_tired()
{
	cli();
	if (button_is_pressed())
	{
		for (double j = 0; j<20000; j++);	// debounce
		// going to sleep
		enter_deepsleep();
	}
	sei();
}

/**
 * @brief Construct a new ISR object
 * Interrupt routine when the MCU wakes up from an on / off button press.
 * 
 */
ISR(INT0_vect, ISR_BLOCK)
{

	disable_on_interrupt();
	sleep_disable();
	wakey_wakey();
	for (double j = 0; j<20000; j++);	// debounce

}

/**
 * @brief Configure INT0 to be an interrupt.
 * 
 */
void enable_on_interrupt()
{
	SET_BIT(GIMSK, INT0);   // Enable External Interrupts Pin change
}

/**
 * @brief Remove INT0 as an interrupt.
 * 
 */
void disable_on_interrupt()
{
	CLEAR_BIT(GIMSK, INT0);	// Disable External Interrupts Pin change
}

/**
 * @brief Poll manually if button is pressed. (active low)
 * 
 * @return bool, true if pressed down, false if not pressed.
 */
bool button_is_pressed()
{
#ifdef INVERT_IO
	return TEST_BIT_SET(PINB, 6);
#else
	return TEST_BIT_CLEAR(PINB, 6);
#endif
}
