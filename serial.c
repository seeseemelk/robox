#include "serial.h"
#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <util/delay_basic.h>
#include <stdio.h>

#define ENABLE_SERIAL  SET_BIT(TIMSK, OCIE1B)		// Enable  Timer 1 output compare interrupt A
#define DISABLE_SERIAL CLEAR_BIT(TIMSK, OCIE1B)		// Disable Timer 1 output compare interrupt A
#define SERIAL_ONGOING TEST_BIT_SET(TIMSK, OCIE1B)

#define SERIAL_RX_HIGH TEST_BIT_SET(PINB, RXPIN)

static u8 s_mask = 0;
static u8 s_byte = 0;
static i8 s_state = 0;
static volatile i16 rx_count = 0;


void serial_init()
{
    SET_BIT(DDRB, TXPIN); // set TX for output
	// CLEAR_BIT(DDRB, RXPIN); // set RX for input

    PORTB |= MASK(TXPIN) | MASK(RXPIN); // assumes no inverse logic

    // Configure Serial speed - configure Timer 1 to call the output-compare match interrupt
    OCR1C = SERIAL_SPEED;	// Timer 1 top value
    TCCR1B = SERIAL_CLOCK;	// Timer 1 clock prescaler

	// send dummy data, first packet swallows the start bit :(
	// transmit_byte('0');
}

/* Serial Transmit */

void transmit_byte(u8 b)
{
	// check previous serial operation is ongoing
	while(SERIAL_ONGOING) _NOP();

	// setup transmission
	cli();
	TCNT1 = 0;
	s_byte = b;
	s_state = START_BIT;
	sei();
	ENABLE_SERIAL;

	// wait untill ongoing transmission is complete
	while(!SERIAL_ONGOING) _NOP();
}

ISR(TIMER1_COMPB_vect, ISR_BLOCK)
{
	switch (s_state)
	{
		case START_BIT:
			CLEAR_BIT(PORTB, TXPIN); // tx pin high, send 1

#ifdef TX_LSB_FIRST
			s_mask = 0b00000001;
#else
			s_mask = 0b10000000;
#endif
			s_state = SEND_DATA;
			break;

		case SEND_DATA:
			if (s_byte & s_mask) // choose bit
				SET_BIT(PORTB, TXPIN); // tx pin high, send 1
			else
				CLEAR_BIT(PORTB, TXPIN); // tx pin low, send 0

#ifdef TX_LSB_FIRST
			s_mask <<= 1;
#else
			s_mask >>= 1;
#endif
			if (s_mask == 0)
				s_state = STOP_BIT;
			break;

		case STOP_BIT:
			SET_BIT(PORTB, TXPIN); // tx pin high, send 1

			s_state = TX_PAUSE;
			break;

		case TX_PAUSE:
			s_state = TX_END;
			break;

		case TX_END:
			DISABLE_SERIAL;
			break;

		default:
			break;
	}
}

void test()
{
	transmit_byte('H');
	transmit_byte('e');
	transmit_byte('l');
	transmit_byte('l');
	transmit_byte('o');
	transmit_byte(' ');
	transmit_byte('w');
	transmit_byte('o');
	transmit_byte('r');
	transmit_byte('l');
	transmit_byte('d');
	transmit_byte('\n');
	transmit_byte('\r');
}

/* Serial Receive */

u8 read_byte()
{
#if defined(SERIAL_DEBUG_SAMPLE_TIMES) || defined(SERIAL_DEBUG_SAMPLE_VALUE)
	bool toggle = false;
#endif
	u8 rx_byte = 0;

	// check previous serial operation is ongoing
	while(SERIAL_ONGOING) _NOP();

	cli();

	while(SERIAL_RX_HIGH);	// wait for start bit
	_delay_loop_1(SERIAL_RX_MIDDLE);
	_delay_loop_1(SERIAL_RX_DELAY);		// skip startbit

#ifdef RX_LSB_FIRST
	for(u8 mask = 0b00000001; mask != 0; mask <<= 1)
#else
	for(u8 mask = 0b10000000; mask != 0; mask >>= 1)
#endif
	{

#ifdef SERIAL_DEBUG_SAMPLE_TIMES
		if (toggle) SET_BIT(PORTB, TXPIN);
		else CLEAR_BIT(PORTB, TXPIN);
		toggle = !toggle;
#elif defined(SERIAL_DEBUG_SAMPLE_VALUE)
		if (SERIAL_RX_HIGH) SET_BIT(PORTB, TXPIN);
		else CLEAR_BIT(PORTB, TXPIN);
#else
		// must keep alignment otherwise timing for debugging features is off
		_NOP();
		_NOP();
		_NOP();
#endif

		if (SERIAL_RX_HIGH)
			rx_byte |= mask;
		else
			_NOP();

		_delay_loop_1(SERIAL_RX_DELAY);
	}
	sei();

	return rx_byte;
}