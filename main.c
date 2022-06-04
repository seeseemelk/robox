#include "adc.h"
#include "audio.h"
#include "battery.h"
#include "button.h"
#include "led.h"
#include "power.h"
#include "defs.h"
#include "serial.h"

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <avr/sleep.h>
#include <stdio.h>

static int serial_putchar(char c, FILE *stream)
{
	if (c == '\n')
		serial_putchar('\r', stream);
	
	transmit_byte(c);

	return 0;
}

static FILE serial_out = FDEV_SETUP_STREAM(serial_putchar, NULL, _FDEV_SETUP_WRITE);



// static void general_init()
// {
// 	cli();
// 	led_init();
// 	power_init();
// 	adc_init();
// 	button_init();
// 	audio_init();
// 	power_enable_ble();

// 	SET_BIT(MCUCR, PUD);	// disable pull-ups
// 	SET_BIT(MCUCR, BODS);	// disable BOD
// 	SET_BIT(MCUCR, BODSE);

// 	CLEAR_BIT(WDTCR, WDE);	// disable watchdog
// 	SET_BIT(WDTCR, WDCE);
// 	sei();
// }

// /* main test audio and led render*/
// int main()
// {
// 	general_init();

// 	while (1)
// 	{
// 		check_if_tired();
// 		battery_update();
// 		audio_render_effects();
// 	}	
// }

/* test main serial */
int main()
{
	cli();
	serial_init();
	sei();

	stdout = &serial_out;

	// bool toggle = false;
	for (double j = 0; j<100000; j++);
	while (1)
	{
		test();
		// printf("Hello world!\n");
	}
}
