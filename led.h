#ifndef _LED_H_
#define _LED_H_

#include "defs.h"
#include <stdbool.h>

typedef struct
{
	u8 r;
	u8 g;
	u8 b;
} Color;

void led_init();
void led_set1(u8 r, u8 g, u8 b);
void led_set2(u8 r, u8 g, u8 b);
void led_enable_scaling();
void led_disable_scaling();
void showRGB();

void led_set_full(bool r1, bool g1, bool b1, bool r2, bool g2, bool b2);

#endif
