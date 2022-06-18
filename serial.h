#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "defs.h"
#include <stdbool.h>
#include <stdio.h>

#define TXPIN PB2
#define RXPIN PB0

/* serial options */
#define SERIAL_115200
// #define SERIAL_57600
#define RX_LSB_FIRST
#define TX_LSB_FIRST
// #define SERIAL_DEBUG_SAMPLE_TIMES
// #define SERIAL_DEBUG_SAMPLE_VALUE


#ifdef SERIAL_115200        // configure Timer +- 115.2kHz @8MHz clock
    #define SERIAL_SPEED 69
    #define SERIAL_CLOCK MASK(CS10)
    #define SERIAL_OFFSET 34
    #define SERIAL_RX_DELAY 19
    #define SERIAL_RX_MIDDLE 10

#elif defined(SERIAL_57600) // configure Timer +- 57.6kHz @8MHz clock
    #define SERIAL_SPEED 138
    #define SERIAL_CLOCK MASK(CS10)
    #define SERIAL_OFFSET 69
    #define SERIAL_RX_DELAY 42
    #define SERIAL_RX_MIDDLE 22

#endif

#define START_BIT    0
#define SEND_DATA    1
#define RECEIVE_DATA 1
#define STOP_BIT     2
#define TX_PAUSE     3
#define TX_END       4

void serial_init();
void transmit_byte(u8 b);
bool test_interrupt();
void test();
u8 read_byte();

// static int serial_putchar(char c, FILE *stream);

#endif