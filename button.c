#include "button.h"

#include "adc.h"
#include "config.h"
#include "defs.h"
#include "power.h"
#include "led.h"
#include "audio.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
// #include <util/delay.h>
#include <util/delay_basic.h>

#define ENABLE_ON_INTERRUPT   SET_BIT(GIMSK, INT0)   //Enable External Interrupts Pin change
#define DISABLE_ON_INTERRUPT  CLEAR_BIT(GIMSK, INT0)
#define ENABLE_TIMER1  SET_BIT(TIMSK, TOIE1)		// Enable  Timer 1 output compare interrupt A
#define DISABLE_TIMER1 CLEAR_BIT(TIMSK, TOIE1)		// Disable Timer 1 output compare interrupt A
#define ONGOING_TIMER1 TEST_BIT_SET(TIMSK, TOIE1)

volatile bool global_light_enable = true;
volatile bool global_music_enable = true;
volatile u16 press_sequence = 0;
volatile u16 press_mask = 0;
volatile bool color[] = {false, true, false, false, true, false};

/**
 * @brief Initialisation function for the mcu on / off button and INT0 interrupt.
 * 
 */
void button_init()
{
	DDRB &= ~(1 << 6);	// configre PB6 as an input
	
	ENABLE_ON_INTERRUPT;
	// level interrupt INT0 (low level)
    MCUCR &= ~((1 << ISC01) | (1 << ISC00));

	OCR1C = 35;	// Timer 1 top value (4.48ms)
    TCCR1B = MASK(CS13) | MASK(CS11) | MASK(CS10);	// Timer 1 clock prescaler
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

	// Digital Input (buffer) Disable Registers
	// DIDR0 &= ~(MASK(ADC6D) | MASK(ADC5D) | MASK(ADC4D) | MASK(ADC3D) | MASK(AREFD) | MASK(ADC2D) | MASK(ADC1D) | MASK(ADC0D));
	// DIDR1 &= ~(MASK(ADC10D) | MASK(ADC9D) | MASK(ADC8D) | MASK(ADC7D));

	adc_init();	// assuming adc is still configured, make adc enable and disable functions
	power_enable_ble();
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

/**
 * @brief Call to place the MCU itself in a low power mode.
 * Be sure to configure an interupt that can wake the MCU from the "Power Down"
 * sleep mode.
 * 
 */
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
u8 button_press_menu()
{
	u8 state = MENU_NOTHING;
	u8 short_presses = 0;

	cli();
	TCNT1 = 0;
	press_sequence = 0x1;
	press_mask = 0b10;
	led_set_full(true, true, true, false, false, false);

	sei();
	ENABLE_TIMER1;
	while (press_mask > 0) led_set_full(false, false, true, false, false, true);
	// DISABLE_TIMER1;
	cli();

	if (button_is_pressed())
		state = MENU_SHUTDOWN;
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
			state = MENU_LIGHT_TOGGLE;
			break;
		
		case 2:
			state = MENU_MUSIC_TOGGLE;
			break;
		
		default:
			break;
		}
	}
	wait_until_depressed();
	return state;
}

/**
 * @brief Function to be used in the main program loop.
 * Checks if on / off button is pressed by polling it.
 * If pressed configure the MCU to go in sleep mode.
 * 
 */
void check_if_tired()
{
	cli();
	if (button_is_pressed())
	{
		switch (button_press_menu())
		{
			case MENU_SHUTDOWN:
				// red + white
				// long press
				color[0] = true;
				color[1] = true;
				color[2] = true;
				color[3] = true;
				color[4] = false;
				color[5] = false;
				// fade_1_time(0xFF, 0x00, 0x00);
				// going to sleep
				// enter_deepsleep();
				// wait_until_depressed();
				break;

			case MENU_LIGHT_TOGGLE:
				// green + white
				// 1x short press
				color[0] = true;
				color[1] = true;
				color[2] = true;
				color[3] = false;
				color[4] = true;
				color[5] = false;
				// fade_1_time(0xFF, 0x00, 0xFF);
				// global_light_enable = !global_light_enable;
				// led_set_full(0, 0, 0, 0, 0, 0);
				break;

			case MENU_MUSIC_TOGGLE:
				// blue + white
				// 2x short press
				color[0] = true;
				color[1] = true;
				color[2] = true;
				color[3] = false;
				color[4] = false;
				color[5] = true;
				// led_set_full(false, true, true, false, true, true);
				// fade_1_time(0x00, 0xFF, 0xFF);
				// global_music_enable = !global_music_enable;
				// if (global_music_enable)
				// 	power_enable_ble();
				// else
				// 	power_disable_ble();
				break;

			case MENU_NOTHING:
			default:
				color[0] = true;
				color[1] = true;
				color[2] = true;
				color[3] = true;
				color[4] = true;
				color[5] = true;
				break;
		}		
	}
	else
		led_set_full(color[0], color[1], color[2], color[3], color[4], color[5]);
	sei();
}

/**
 * @brief Construct a new ISR object
 * Interrupt routine when the MCU wakes up from an on / off button press.
 * 
 */
ISR(INT0_vect, ISR_BLOCK)
{

	DISABLE_ON_INTERRUPT;
	sleep_disable();
	wakey_wakey();
	// for (double j = 0; j<20000; j++);	// debounce

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

ISR(TIMER1_OVF_vect, ISR_BLOCK)
{
	TCNT1 = 0;
	if (button_is_pressed())
		press_sequence |= press_mask;

	press_mask <<= 1;

	if (press_mask == 0)
		DISABLE_TIMER1;
}