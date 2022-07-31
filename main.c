#include "adc.h"
#include "audio.h"
#include "battery.h"
#include "button.h"
#include "led.h"
#include "power.h"
#include "defs.h"
#include "timer1.h"

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <avr/sleep.h>

void general_init()
{
	cli();
	led_init();
	power_init();
	adc_init();
	battery_init();
	button_init();
	audio_init();
	power_enable_ble();
	setup_beat_detection_counter();

	SET_BIT(MCUCR, PUD);	// disable pull-ups
	SET_BIT(MCUCR, BODS);	// disable BOD
	SET_BIT(MCUCR, BODSE);

	CLEAR_BIT(WDTCR, WDE);	// disable watchdog
	SET_BIT(WDTCR, WDCE);
	sei();
}

/* main test audio and led render*/
int main()
{
	general_init();

	// cli();
	// setup_25ms_interrupt();
	// sei();

	// global_modus = modus_night_light;

	SET_BIT(DDRB, PB0);
	while (1)
	{
		button_menu();
		// led_init();
		battery_update();
		if (global_modus == modus_night_light)
		{
			showRGB();
		}
		else if (global_modus == modus_normal)
		{
			// led_set_full(0x12);
		// 	setup_beat_detection_counter();
			audio_render_effects();
		}
	}
}
