#include "led.h"
#include "defs.h"

#include <avr/io.h>

#define LED1_R 2
#define LED1_G 1
#define LED1_B 0

#define LED2_R 6
#define LED2_G 5
#define LED2_B 4

typedef struct
{
	int r;
	int g;
	int b;
} Color;

static Color s_led1;
static Color s_led2;

void led_init()
{
	DDRA = MASK(LED1_R) | MASK(LED1_G) | MASK(LED1_B)
	     | MASK(LED2_R) | MASK(LED2_G) | MASK(LED2_B);
	PORTA |= (MASK(LED1_R) | MASK(LED1_G) | MASK(LED1_B)
		     | MASK(LED2_R) | MASK(LED2_G) | MASK(LED2_B));
}

void led_set1(int r, int g, int b)
{
	s_led1.r = r;
	s_led1.g = g;
	s_led1.b = b;
}

void led_set2(int r, int g, int b)
{
	s_led2.r = r;
	s_led2.g = g;
	s_led2.b = b;
}
