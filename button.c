#include "button.h"

#include "defs.h"

#include <avr/io.h>

void button_init()
{
}

bool button_is_pressed()
{
	return !TEST_BIT_SET(PINB, 6);
}
