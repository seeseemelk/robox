#include "adc.h"
#include "audio.h"
#include "button.h"
#include "led.h"
#include "power.h"

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <avr/sleep.h>



// const i8 s_wave[] PROGMEM =
// {
// 	//1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
// 	 0,  0,  0,  0,  0,  0,  0,  0,  0,
// 	 0,  0,  0,  0,  0,  0,  0,  0,  0,
// 	 1,  1,  2,  4,  8, 16, 32, 48, 63,
// };
// #define WAVE_OFFSET (sizeof(s_wave) / 6)
//
//i8 waveAt(i8 point)
//{
//	i8 index = point % sizeof(s_wave);
//	if (((point / sizeof(s_wave)) & 1) == 0)
//		return pgm_read_byte_near(s_wave + index);
//	else
//		return pgm_read_byte_near(s_wave + sizeof(s_wave) - index - 1);
//}

void sleep() 
{
	// cli();
	// ADCSRA = 0;                   // ADC off
	
	// sleep_enable();
	// set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	// sei();
	// sleep_cpu();
	////cli();
	////ADCSRA |= _BV(ADEN);                    // ADC on
	////sei();                                  // Enable interrupts
} // sleep
/*
ISR(PCINT0_vect) {
	if ((PINB&(1<<6)) == 0) // Reading the Pin Value ==> pb pressedµ
	{
		
	}
}
*/

/*
	general_init - general init of all functions needed
	To be called when the mcu starts.
*/


void general_init()
{
	cli();
	led_init();
	power_init();
	adc_init();
	button_init();
	audio_init();
	power_enable_ble();
	power_enable_amp();
	sei();
}

/*
	music - main routine that powers all functionality.
*/
void music()
{
	// audio_render_effects();
	/*for (int i = 15; i >= 0; i--)
	{
		led_set2(15, i, 15);
		led_set1(15, i, 15);
		for (int j=0;j<20000;j++);
	}
	for (int i = 0; i < 15; i++)
	{
		led_set2(15, i, 15);
		led_set1(15, i, 15);
		for (int j=0;j<20000;j++);
	}
	for (int i = 15; i >= 0; i--)
	{
		led_set2(i, 15, 15);
		led_set1(i, 15, 15);
		for (int j=0;j<20000;j++);
	}
	for (int i = 0; i < 15; i++)
	{
		led_set2(i, 15, 15);
		led_set1(i, 15, 15);
		for (int j=0;j<20000;j++);
	}
	for (int i = 15; i >= 0; i--)
	{
		led_set2(15, 15, i);
		led_set1(15, 15, i);
		for (int j=0;j<20000;j++);
	}
	for (int i = 0; i < 15; i++)
	{
		led_set2(15, 15, i);
		led_set1(15, 15, i);
		for (int j=0;j<20000;j++);
	}*/
	
	/*
	//audio_render_effects();
	for (int i = 0; i < 255; i++)
	{
		led_set2(255, i, 255);
		led_set1(255, i, 255);
		for (int j=0;j<20000;j++);
	}

	for (int i = 0; i < 255; i++)
	{
		led_set2(i, 255, 255);
		led_set1(i, 255, 255);
		for (int j=0;j<20000;j++);
	}
	for (int i = 0; i < 255; i++)
	{
		led_set2(255, 255, i);
		led_set1(255, 255, i);
		for (int j=0;j<20000;j++);
	}*/
}

/* combined main */

// int main()//int argc, char** argv)
// {
// 	// int powerState = 0;
// 	// int pbPress = 0;

// 	general_init();

// 	while (1)
// 	{
// 		// powerState - power on button, located in button.c
		
// 		if( powerState == 0 ) nap_time();
// 		else music();

// 	}
// }

/*********************************/

/* main test wakeup - shutdown */
// int main()
// {
// 	general_init();

// 	while (1)
// 	{
// 		if (powerState == 0)
// 		{
// 			led_set_full(1, 0, 0, 1, 0, 0);
// 		}
// 		else if (powerState == 1)
// 		{
// 			led_set_full(0, 1, 0, 0, 1, 0);
// 		}
// 		else
// 		{
// 			led_set_full(0, 0, 1, 0, 0, 1);
// 		}


// 		cli();
// 		if (button_is_pressed())
// 		{
// 			for (double j = 0; j<20000; j++);	// debounce
// 			enable_on_interrupt();
// 			// going to sleep
// 			led_set_full(0, 0, 0, 0, 0, 0);

// 			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
// 			sleep_enable();
// 			nap_time();
// 			sei();
// 			sleep_cpu();
// 		}
// 		sei();
// 	}
// }

// ISR(INT0_vect, ISR_BLOCK)
// {
// 	disable_on_interrupt();
// 	powerState++;
// 	sleep_disable();
// 	wakey_wakey();
// 	if (powerState > 2) powerState = 0;
// 	for (double j = 0; j<20000; j++);	// debounce
// }
/*********************************/

/* main test audio and led render*/
int main()
{
	// general_init();
	cli();
	power_init();
	led_init();
	adc_init();
	audio_init();
	sei();
	power_enable_ble();

	while (1)
	{
		// led_set_full(1, 1, 1, 1, 1, 1);
		// led_set1(0, 32, 63);
		// _delay_ms(500);
		// led_set_full(0, 0, 0, 0, 0, 0);
		// led_set1(0, 32, 63);
		// _delay_ms(500);
		// for (double j = 0; j<200000; j++);

		// power_disable_ble();
		// led_set1(63, 63, 63);
		// for (double j = 0; j<200000; j++);
		// // _delay_ms(5000);
		// _delay_ms(100);
		audio_render_effects();
		// __asm__ __volatile__ ("nop");
	}	
}