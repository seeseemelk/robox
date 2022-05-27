#include "adc.h"
#include "audio.h"
#include "battery.h"
#include "button.h"
#include "led.h"
#include "power.h"

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <avr/sleep.h>


static void general_init()
{
	cli();
	led_init();
	power_init();
	adc_init();
	button_init();
	audio_init();
	power_enable_ble();
	sei();
}

/* main test audio and led render*/
int main()
{
	general_init();

	while (1)
	{
		battery_update();
		audio_render_effects();
	}	
}
