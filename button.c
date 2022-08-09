#include "button.h"

#include "adc.h"
#include "config.h"
#include "defs.h"
#include "power.h"
#include "led.h"
#include "audio.h"
#include "timer1.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define ENABLE_ON_INTERRUPT   SET_BIT(GIMSK, INT0)   //Enable External Interrupts Pin change
#define DISABLE_ON_INTERRUPT  CLEAR_BIT(GIMSK, INT0)

volatile GlobalModus global_modus = mapper_normal_mode;
volatile u16 night_light_counter = 0;
volatile u16 press_sequence = 0;
volatile u16 press_mask = 0;

/**
 * @brief Initialisation function for the mcu on / off button and INT0 interrupt.
 * 
 */
void button_init()
{
	DDRB &= ~(1 << 6);	// configre PB6 as an input
	
	// ENABLE_ON_INTERRUPT;
	// level interrupt INT0 (low level)
    MCUCR &= ~((1 << ISC01) | (1 << ISC00));
}

/**
 * @brief Function to be called when the interupt awakes the mcu from sleeping.
 * It will powerup all mcu modules + bluetooth module and amp.
 * 
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
	adc_init();	// assuming adc is still configured, make adc enable and disable functions
	power_enable_ble();

	// toggle_music_only = true;
	// toggle_nightlight = false;

	// led_set_full(0xFF);

	// Digital Input (buffer) Disable Registers
	// DIDR0 &= ~(MASK(ADC6D) | MASK(ADC5D) | MASK(ADC4D) | MASK(ADC3D) | MASK(AREFD) | MASK(ADC2D) | MASK(ADC1D) | MASK(ADC0D));
	// DIDR1 &= ~(MASK(ADC10D) | MASK(ADC9D) | MASK(ADC8D) | MASK(ADC7D));
}

/**
 * @brief Function to be called when the off command is given.
 * Places the mcu and oher components in low power mode.
 * 
 */
void nap_time()
{
	// set lower clock speed
	// SET_BIT(CLKPR, CLKPS3);
	// SET_BIT(CLKPR, CLKPCE);

	power_disable_ble();
	led_set_full(0x00);

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

/**
 * @brief Call to place the MCU itself in a low power mode.
 * Be sure to configure an interupt that can wake the MCU from the "Power Down"
 * sleep mode.
 * 
 */
void enter_deepsleep()
{
	global_modus = mapper_shutdown;
	disable_timer1();
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

/**
 * @brief Function that returns only if the button is released (after being pressed down)
 * Also does some debouncing.
 * 
 * @return u16 
 */
static void wait_until_depressed()
{
	u8 count = 0;
	while (count < 0xFF)
	{
		if (button_is_pressed())
			count = 0;
		else
			count++;
	}
}

/**
 * @brief Detection mechanism long and x-amount of short presses
 * 
 * @return MenuState 
 */
ButtonPressPattern button_press_menu()
{
	GlobalModus state = press_nothing;
	u8 short_presses = 0;

	cli();
	press_sequence = 0;
	press_mask = 0b1;

	setup_button_menu();
	sei();
	while (press_mask > 0) led_set_full(0x44);
	cli();

	led_set_full(0x00);

	if (button_is_pressed())
		state = press_long;
	else
	{
		for (u16 i = 0; i<12; i++)
		{
			if (((press_sequence >> i) & 0xF) == 0b0011)
				short_presses++;
		}

		switch (short_presses)
		{
		case 1:
			state = press_1_short;
			break;
		case 2:
			state = press_2_short;
			break;
		default:
			state = press_nothing;
			break;
		}
	}

	return state;
}

void modus_mapper(ButtonPressPattern state)
{
	u8 color = 0;
	switch (state)
	{
	case press_long:
		global_modus = mapper_shutdown;
		// red + white
		color = MASK(4) | MASK(2) | MASK(1) | MASK(0);
		break;
	
	case press_1_short:
		if (global_modus == mapper_normal_mode)
		{
			global_modus = mapper_music_only;
			// blue + white
			color = MASK(6) | MASK(2) | MASK(1) | MASK(0);
		}
		else if (global_modus == mapper_music_only)
			global_modus = mapper_music_rgb;
		else
		{
			global_modus = mapper_normal_mode;
			// green + white
			color = MASK(5) | MASK(2) | MASK(1) | MASK(0);
		}
		break;

	case press_2_short:
		if (global_modus == mapper_normal_mode)
			global_modus = mapper_music_night_light;
		else if (global_modus == mapper_music_night_light)
			global_modus = mapper_night_light;
		else
		{
			global_modus = mapper_normal_mode;
			// green + white
			color = MASK(5) | MASK(2) | MASK(1) | MASK(0);
		}
		break;
		
	default:
		break;
	}

	if (color != 0)
	{
		wait_until_depressed();
		led_set_full(color);

		setup_25ms_interrupt();
		sei();
		while(counter_25ms < WAIT_1S);
		cli();
	}

	led_set_full(0x00);
}

/**
 * @brief Function to be used in the main program loop.
 * Checks if on / off button is pressed by polling it.
 * If pressed configure the MCU to go in sleep mode.
 * 
 */
void button_menu()
{
	cli();

	if (button_is_pressed())
	{
		disable_timer1();		
		modus_mapper(
			button_press_menu()
		);
		disable_timer1();

		switch (global_modus)
		{
			case mapper_normal_mode:
				wakey_wakey();
				setup_beat_detection_counter();
				break;

			case mapper_shutdown:
				// going to sleep
				enter_deepsleep();
				break;

			case mapper_night_light:
				night_light_counter = 0;
	
				setup_25ms_interrupt();

				// disable audio
				power_disable_ble();
				break;

			case mapper_music_night_light:
				night_light_counter = 0;
	
				setup_25ms_interrupt();

				// disable audio
				power_enable_ble();
				break;

			case mapper_music_rgb:
				setup_25ms_interrupt();
				
				// enable audio
				power_enable_ble();
				break;

			case mapper_music_only:
				led_set_full(0x00);

				// disable audio
				power_enable_ble();
				break;

			default:
				break;
		}		
	}

	// nightlight sleep check
	if (night_light_counter >= WAIT_15M)
	{
		if ((global_modus == mapper_night_light) || (global_modus == mapper_music_night_light))
			enter_deepsleep();
	}

	sei();
}

/**
 * @brief Construct a new ISR object
 * Interrupt routine when the MCU wakes up from an on / off button press.
 * 
 */
ISR(INT0_vect)
{
	DISABLE_ON_INTERRUPT;
	sleep_disable();
	wakey_wakey();
	global_modus = mapper_normal_mode;
	
	wait_until_depressed();
}


/**
 * @brief Poll manually if button is pressed. (active low)
 * 
 * @return bool, true if pressed down, false if not pressed.
 */
bool button_is_pressed()
{
#ifdef INVERT_IO
	return TEST_BIT_SET(PINB, 6);
#else
	return TEST_BIT_CLEAR(PINB, 6);
#endif
}

ISR(TIMER1_OVF_vect)
{
	TCNT1 = 0;
	if (button_is_pressed())
		press_sequence |= press_mask;

	press_mask <<= 1;

	if (press_mask == 0)
		disable_timer1();
}