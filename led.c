#include "led.h"

#include "config.h"
#include "defs.h"

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

static Color s_led1;
static Color s_led2;
static Color _s_led1;
static Color _s_led2;
static bool s_scale = false;

static unsigned int s_counter;

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

static u8 convertBrightness(u8 value, u8 scale)
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

void led_set_full(bool r1, bool g1, bool b1, bool r2, bool g2, bool b2)
{
	uint8_t mask = LED_MASK_INIT;

	if (r1)
		ENABLE_LED(mask, LED1_R);
	if (g1)
		ENABLE_LED(mask, LED1_G);
	if (b1)
		ENABLE_LED(mask, LED1_B);

	if (r2)
		ENABLE_LED(mask, LED2_R);
	if (g2)
		ENABLE_LED(mask, LED2_G);
	if (b2)
		ENABLE_LED(mask, LED2_B);

	PORTA = (PORTA & ~PIN_MASKS) | mask;
}

void led_set1(u8 r, u8 g, u8 b)
{
	i16 scale = 63;
	if (s_scale)
		scale = scale_brightness_to_max(r, g, b);
	s_led1.r = convertBrightness(r, scale);
	s_led1.g = convertBrightness(g, scale);
	s_led1.b = convertBrightness(b, scale);
}

void led_set2(u8 r, u8 g, u8 b)
{
	i16 scale = 63;
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
	PORTA = (PORTA & ~PIN_MASKS) | mask;
}
