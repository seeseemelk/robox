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
#include <util/delay_basic.h>

#define ENABLE_ON_INTERRUPT   SET_BIT(GIMSK, INT0);   //Enable External Interrupts Pin change
#define DISABLE_ON_INTERRUPT  CLEAR_BIT(GIMSK, INT0);

/*
	wakey_wakey - function to be called,
	when the interupt awakes the mcu from sleeping 
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

/*
	nap_time - going back to sleep if off command is given.
	Places the mcu and oher components in low power mode.
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

void enter_deepsleep()
{
	cli();
	ENABLE_ON_INTERRUPT;
	DIDR0 = MASK(AREFD);
	adc_stop();
	set_sleep_mode(SLEEP_MODE_STANDBY);
	sleep_bod_disable();
	sleep_enable();
	nap_time();
	sei();
	sleep_cpu();
}

static void wait_until_depressed()
{
	u8 count = 0;
	while (count < 128)
	{
		if (button_is_pressed())
			count = 0;
		else
			count++;
	}
}

void check_if_tired()
{
	cli();
	if (button_is_pressed())
	{
		wait_until_depressed();
		// for (u16 j = 0; j<65535; j++)
		// 	for (u16 k = 0; k<65535; k++);	// debounce
//		for(u8 j = 0; j<10; j++)
//			_delay_loop_2(65535);	// 261.2 ms / 8 (running at 8MHz instead of 1MHz)
		// for (double j = 0; j<20000; j++);
		// going to sleep
		enter_deepsleep();
		wait_until_depressed();
	}
	sei();
}

ISR(INT0_vect, ISR_BLOCK)
{

	DISABLE_ON_INTERRUPT;
	// powerState++;
	sleep_disable();
	wakey_wakey();
	// if (powerState > 2) powerState = 0;
//	for(u8 j = 0; j<10; j++)
//		_delay_loop_2(65535);	// 261.2 ms / 8 (running at 8MHz instead of 1MHz)
	// for (double j = 0; j<20000; j++);
	// 	for (u16 k = 0; k<65535; k++);	// debounce

}

void button_init()
{
	DDRB &= ~(1 << 6);	// configre PB6 as an input
	
	// level interrupt INT0 (low level)
    MCUCR &= ~((1 << ISC01) | (1 << ISC00));
	ENABLE_ON_INTERRUPT;
}

/*
	button_is_pressed - poll manually if button is pressed.
	(active low)
*/
bool button_is_pressed()
{
#ifdef INVERT_IO
	return TEST_BIT_SET(PINB, 6);
#else
	return TEST_BIT_CLEAR(PINB, 6);
#endif
}

// void on_button_interrupt()
// {
// 	cli();
// 	s_powerState = !s_powerState;

// // 	// debounce
// // 	_delay_ms(100);

// 	if (s_powerState)
// 	{
// 		// prepare for power up
// 		sleep_disable();
// 		sei();
// 	}
// }
		
// 		wakey_wakey();

// 	} else {
// 		// prepare for sleep
// 		nap_time();

// 		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
// 		cli();
// 		sleep_enable();
// 		sei();
// 		sleep_cpu();
// 	}

// }

// ISR(INT0_vect)
// {
//  	on_button_interrupt();
// }
