#include "led.h"
#include "config.h"
#include "defs.h"
#include "timer1.h"

#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#define LED1_R PORTA2
#define LED1_G PORTA1
#define LED1_B PORTA0

#define LED2_R PORTA6
#define LED2_G PORTA5
#define LED2_B PORTA4

#define PIN_MASKS (MASK(LED1_R) | MASK(LED1_G) | MASK(LED1_B) \
	     | MASK(LED2_R) | MASK(LED2_G) | MASK(LED2_B))

#ifdef INVERT_IO
#define LED_MASK_INIT PIN_MASKS
#define ENABLE_LED(var, bit) CLEAR_BIT(var, bit)
#else
#define LED_MASK_INIT 0
#define ENABLE_LED(var, bit) SET_BIT(var, bit)
#endif

#define LED_MODUS_PWM 0
#define LED_MODUS_FULL 1

static Color s_led1;
static Color s_led2;
static Color _s_led1;
static Color _s_led2;
static bool s_scale = false;

static u8 s_counter;
static u16 rgb_counter = 0;
volatile u8 led_modus = LED_MODUS_PWM;

void led_init()
{
	// Configure the LEDs as outputs
	DDRA |= PIN_MASKS;
	// Turns the LEDs off
	PORTA &= ~PIN_MASKS;

	// Configure Timer 0 to call the output-compare match interrupt
	// Enable Timer 0 output compare interrupt A
	TIMSK = MASK(OCIE0A);
	// Configure the output compare register
	OCR0A = 4;
	// Reset timer on output compare
	TCCR0A = MASK(WGM00);
	// Prescaler: clkIO / 64
	TCCR0B = MASK(CS01) | MASK(CS00);
	// TCCR0B = MASK(CS02);
	PORTA |= PIN_MASKS;
}

u8 convertBrightness(u8 value, u8 scale)
{
	u16 longValue = value;
	return (longValue * 63 / scale) & 0x3F;
}

u8 scale_brightness_to_max(u8 r, u8 g, u8 b)
{
	u8 max = r;

	if (g > max) max = g;
	if (b > max) max = b;

	return max;
}


/**
 * @brief Set leds to full brightness.
 * @param color 
 * Least significant nibble: led 1
 * Most significant nibble: led 2
 * 
 * nibble bit 1: red component
 * nibble bit 2: green component
 * nibble bit 3: blue component
 */
void led_set_full(u8 color)
{
	uint8_t mask = LED_MASK_INIT;	

	led_modus = LED_MODUS_FULL;
	if (TEST_BIT_SET(color, 0))
		ENABLE_LED(mask, LED1_R);
	if (TEST_BIT_SET(color, 1))
		ENABLE_LED(mask, LED1_G);
	if (TEST_BIT_SET(color, 2))
		ENABLE_LED(mask, LED1_B);

	if (TEST_BIT_SET(color, 4))
		ENABLE_LED(mask, LED2_R);
	if (TEST_BIT_SET(color, 5))
		ENABLE_LED(mask, LED2_G);
	if (TEST_BIT_SET(color, 6))
		ENABLE_LED(mask, LED2_B);

	PORTA = (PORTA & ~PIN_MASKS) | mask;
}

void led_set1(u8 r, u8 g, u8 b)
{
	i16 scale = 63;

	led_modus = LED_MODUS_PWM;
	if (s_scale)
		scale = scale_brightness_to_max(r, g, b);
	s_led1.r = convertBrightness(r, scale);
	s_led1.g = convertBrightness(g, scale);
	s_led1.b = convertBrightness(b, scale);
}

void led_set2(u8 r, u8 g, u8 b)
{
	i16 scale = 63;

	led_modus = LED_MODUS_PWM;
	if (s_scale)
		scale = scale_brightness_to_max(r, g, b);
	s_led2.r = convertBrightness(r, scale);
	s_led2.g = convertBrightness(g, scale);
	s_led2.b = convertBrightness(b, scale);
}

void led_enable_scaling()
{
	s_scale = true;
}

void led_disable_scaling()
{
	s_scale = false;
}

void showRGB()
{
	Color rgb = {0, 0, 0};
	// https://uwaterloo.ca/women-in-engineering/sites/ca.women-in-engineering/files/uploads/files/rgb_led.pdf
	rgb_counter += 1;

	u8 _c = rgb_counter & 0x3F;
	u8 _p = rgb_counter >> 6;
	switch (_p)
	{
	case 0:
		rgb.r = 0x3F - _c; // red goes from on to off
		rgb.g = _c; // green goes from off to on
		rgb.b = 0; // blue is always off
		break;

	case 1:
		rgb.r = 0; // red is always off
		rgb.g = 0x3F - _c; // green on to off
		rgb.b = _c; // blue off to on
		break;

	case 2:
		rgb.r = _c; // red off to on
		rgb.g = 0; // green is always off
		rgb.b = 0x3F - _c; // blue on to off
		break;
	
	default:
		rgb_counter = 0;
		break;
	}

	if (rgb_counter >= 192)
		rgb_counter = 0;

	// led_set1(rgb.r, rgb.g, rgb.b);
	// led_set2(rgb.r, rgb.g, rgb.b);

	s_led1.r = s_led2.r = rgb.r;
	s_led1.g = s_led2.g = rgb.g;
	s_led1.b = s_led2.b = rgb.b;
	led_modus = LED_MODUS_PWM;

	setup_25ms_interrupt();
	while (counter_25ms < WAIT_100MS);
}

ISR(TIMER0_COMPA_vect)
{
	uint8_t mask = LED_MASK_INIT;
	if (s_counter < _s_led1.r)
		ENABLE_LED(mask, LED1_R);
	if (s_counter < _s_led1.g)
		ENABLE_LED(mask, LED1_G);
	if (s_counter < _s_led1.b)
		ENABLE_LED(mask, LED1_B);

	if (s_counter < _s_led2.r)
		ENABLE_LED(mask, LED2_R);
	if (s_counter < _s_led2.g)
		ENABLE_LED(mask, LED2_G);
	if (s_counter < _s_led2.b)
		ENABLE_LED(mask, LED2_B);

	s_counter = (s_counter - 1) & 0x3F;

	if (s_counter == 0)
	{
		_s_led1 = s_led1;
		_s_led2 = s_led2;
	}
	// if (led_modus == LED_MODUS_PWM)
		PORTA = (PORTA & ~PIN_MASKS) | mask;
}
