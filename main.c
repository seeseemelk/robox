#include "adc.h"
#include "audio.h"
#include "battery.h"
#include "button.h"
#include "led.h"
#include "power.h"
#include "defs.h"
#include "serial.h"

// #include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>

// #include <avr/sleep.h>
#include <avr/cpufunc.h>
#include <stdio.h>

static int serial_putchar(char c, FILE *stream);

static int serial_putchar(char c, FILE *stream)
{
	// SET_BIT(PORTB, TXPIN); // tx pin high, send 1
	// _delay_loop_1(0);
	// CLEAR_BIT(PORTB, TXPIN);
	// _delay_loop_1(0);
	if (c == '\n')
		serial_putchar('\r', stream);
	
	transmit_byte(((u8)c));

	return 0;
}

static int serial_readchar();

static int serial_readchar()
{
	return read_byte();
}

static FILE serial_stream = FDEV_SETUP_STREAM(serial_putchar, serial_readchar, _FDEV_SETUP_RW);

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
	stdout = &serial_stream;
	stdin = &serial_stream;
	stderr = &serial_stream;
	while (1)
	{
		// serial loopback
		read = read_byte();
		transmit_byte(read);

		// test();
		// transmit_byte('s');
		// transmit_byte('\n');
		// transmit_byte('\r');
		// transmit_byte('d');
		// transmit_byte('\n');
		// transmit_byte('\r');
		// scanf("n%"PRIx8"\n", &read);
		// scanf("\r");
		// transmit_byte(read);
		// printf("Hello world!\n");
		// fprintf(stdout,"Hello world!\n");
		// transmit_byte('f');
		// transmit_byte('\n');
		// transmit_byte('\r');
		// SET_BIT(PORTB, TXPIN); // tx pin high, send 1
		// _delay_loop_1(0);
		// CLEAR_BIT(PORTB, TXPIN);
		// _delay_loop_1(0);
		// for (double j = 0; j<100000; j++);
		// test();
		// sprintf(stdout, "Hello world!\n");
		// fprintf(&serial_out, "%f\n", 3.1415);
	}
}
