#include "adc.h"
#include "audio.h"
#include "battery.h"
#include "button.h"
#include "led.h"
#include "power.h"

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*
	general_init - general init of all functions needed
	To be called when the mcu starts.
*/
static void general_init()
{
	cli();
	led_init();
	power_init();
	adc_init();
	power_disable_amp();
	button_init();
	audio_init();
	battery_init();
	sei();
}

/* main test audio and led render*/
int main()
{
	general_init();

	while (1)
	{
//		battery_update();
		audio_render_effects();
	}	
}
