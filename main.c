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
#include <avr/cpufunc.h>
#include <stdio.h>

// static int serial_putchar(char c, FILE *stream);

// static int serial_putchar(char c, FILE *stream)
// {
// 	if (c == '\n')
// 		serial_putchar('\r', stream);
	
// 	transmit_byte(c);

// 	return 0;
// }

// static FILE serial_output = FDEV_SETUP_STREAM(serial_putchar, NULL, _FDEV_SETUP_WRITE);

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
	u8 read;

	cli();
	serial_init();
	sei();
	while (1)
	{
		read = read_byte();
		transmit_byte(read);

		// for (double j = 0; j<100000; j++);
		// test();
		// sprintf(stdout, "Hello world!\n");
		// printf("Hello world!\n");
		// fprintf(&serial_out, "%f\n", 3.1415);
	}
}
