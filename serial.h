#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "defs.h"
#include <stdbool.h>
#include <stdio.h>

#define TXPIN PB2
#define RXPIN PB0

#define SERIAL_115200



#ifdef SERIAL_115200

// configure Timer +- 115.2kHz @8MHz clock
#define SERIAL_SPEED 69
#define SERIAL_CLOCK MASK(CS10)

#elif defined(SERIAL_57600)

// configure Timer +- 57.6kHz @8MHz clock
#define SERIAL_SPEED 138
#define SERIAL_CLOCK MASK(CS10)

#endif

void serial_init();
void transmit_byte(u8 b);
bool test_interrupt();
void test();

// static int serial_putchar(char c, FILE *stsream);

#endif