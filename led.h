#ifndef _LED_H_
#define _LED_H_

#define TICKS_20MS_30S 1500

#include "defs.h"
#include <stdbool.h>

typedef struct
{
	u8 r;
	u8 g;
	u8 b;
} Color;

void led_init();
void led_set(volatile Color *led, u8 r, u8 g, u8 b, bool show_white);
void led_enable_scaling();
void led_disable_scaling();
void showRGB();

void led_set_full(u8 color);

extern volatile u16 ticks_20ms;
extern volatile Color led1;
extern volatile Color led2;

#endif
