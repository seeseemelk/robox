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

	while (1)
	{
		button_menu();
		battery_update();
		if (global_night_light_enable)
			showRGB();
		else
		{
			setup_beat_detection_counter();
			audio_render_effects();
		}
	}
}
