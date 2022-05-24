#include "button.h"
#include "adc.h"
#include "power.h"
#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

bool powerState = 0;

/*
	wakey_wakey - function to be called,
	when the interupt awakes the mcu from sleeping 
*/
void wakey_wakey()
{
	adc_init();	// assuming adc is still configured, make adc enable and disable functions
	power_enable_ble();
	power_enable_amp();
	/*
	sei();
    ADCSRA |= _BV(ADEN);                    // ADC on
	*/
}

/*
	nap_time - going back to sleep if off command is given.
	Places the mcu and oher components in low power mode.
*/
void nap_time()
{
	power_disable_amp();
	power_disable_ble();
	//led_off();
	adc_stop();
}

void button_init()
{
	// SET_BIT(DDRB, DDB6); // configre PB6 as an input
	DDRB &= ~(1 << 6);	// configre PB6 as an input
	
	// SET_BIT(GIMSK, INT0);   //Enable External Interrupts INT0 and INT1
	// SET_BIT(MCUCR, ISC01);	// falling edge

	/*
	cli();
	GIMSK = 0b00100000;	//enable pin change interrupt
	PCMSK = 0b00000111;	//enable interrupt on gpioxxxxx
	*/
}

/*
	button_is_pressed - poll manually if button is pressed.
	(active low)
*/
bool button_is_pressed()
{
	return !TEST_BIT_SET(PINB, 6);
}

void on_button_interrupt()
{
	cli();
	powerState = !powerState;

	// debounce
	_delay_ms(100);

	if (powerState)
	{
		// prepare for power up
		sleep_disable();
		sei();
		
		wakey_wakey();

	} else {
		// prepare for sleep
		nap_time();

		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		cli();
		sleep_enable();
		sei();
		sleep_cpu();
	}

}

ISR(INT0_vect)
{
	on_button_interrupt();
}
