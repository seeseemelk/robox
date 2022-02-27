#include "led.h"
#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define LED1_R 2
#define LED1_G 1
#define LED1_B 0

#define LED2_R 6
#define LED2_G 5
#define LED2_B 4

#define PIN_MASKS (MASK(LED1_R) | MASK(LED1_G) | MASK(LED1_B) \
	     | MASK(LED2_R) | MASK(LED2_G) | MASK(LED2_B))

typedef struct
{
	u8 r;
	u8 g;
	u8 b;
} Color;

static Color s_led1;
static Color s_led2;

static unsigned int s_counter;

void led_init()
{
	// Configure the LEDs as outputs
	DDRA |= PIN_MASKS;
	// Turns the LEDs off
	PORTA |= PIN_MASKS;

	// Configure Timer 0 to call the output-compare match interrupt
	// Enable Timer 0 output compare interrupt A
	TIMSK = MASK(OCIE0A);
	// Configure the output compare register
	OCR0A = 4;
	// Reset timer on output compare
	TCCR0A = MASK(WGM00);
	// Prescaler: clkIO / 256
	TCCR0B = MASK(CS02);
}

static u8 convertBrightness(u8 value)
{
	return value & 0xF;
}

void led_set1(u8 r, u8 g, u8 b)
{
	s_led1.r = convertBrightness(r);
	s_led1.g = convertBrightness(g);
	s_led1.b = convertBrightness(b);
}

void led_set2(u8 r, u8 g, u8 b)
{
	s_led2.r = convertBrightness(r);
	s_led2.g = convertBrightness(g);
	s_led2.b = convertBrightness(b);
}

ISR(TIMER0_COMPA_vect)
{
	uint8_t mask = PIN_MASKS;
	if (s_counter < s_led1.r)
		CLEAR_BIT(mask, LED1_R);
	if (s_counter < s_led1.g)
		CLEAR_BIT(mask, LED1_G);
	if (s_counter < s_led1.b)
		CLEAR_BIT(mask, LED1_B);

	if (s_counter < s_led2.r)
		CLEAR_BIT(mask, LED2_R);
	if (s_counter < s_led2.g)
		CLEAR_BIT(mask, LED2_G);
	if (s_counter < s_led2.b)
		CLEAR_BIT(mask, LED2_B);

	s_counter = (s_counter - 1) & 0xF;
	PORTA = (PORTA & ~PIN_MASKS) | mask;
}
