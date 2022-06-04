#include "serial.h"
#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>


static u8 tx_mask = 0;
static u8 tx_byte = 0;
static i8 tx_state = 0;


void serial_init()
{
    SET_BIT(DDRB, TXPIN); // set TX for output
	// CLEAR_BIT(DDRB, RXPIN); // set RX for input

	// debugging (serial clk)
	// SET_BIT(DDRB, RXPIN); // set RX for output


    PORTB |= MASK(TXPIN)|MASK(RXPIN); // assumes no inverse logic

    // Configure Timer 1 to call the output-compare match interrupt

	// top of counter
    OCR1C = SERIAL_SPEED;


	// Prescaler: clkIO / 1
    TCCR1B = SERIAL_CLOCK;

	// first packet swallows the start bit :(
	// send dummy data
	transmit_byte('0');
}

void enable_serial()
{
	// Enable Timer 1 output compare interrupt A
	SET_BIT(TIMSK, OCIE1B);
}

void disable_serial()
{
	// Enable Timer 1 output compare interrupt A
	CLEAR_BIT(TIMSK, OCIE1B);
}

bool test_interrupt()
{
	return TEST_BIT_SET(TIMSK, OCIE1B);
}

void transmit_byte(u8 b)
{
	// check previous transmission ongoing
	while(test_interrupt()) _NOP();

	// setup transmission
	tx_byte = b;

	// serial write
	tx_state = 0;

	enable_serial();

	// wait untill ongoing transmission is complete
	// while(test_interrupt()) _NOP();
}


ISR(TIMER1_COMPB_vect, ISR_BLOCK)
{
	switch (tx_state)
	{
		case 0:
			CLEAR_BIT(PORTB, TXPIN); // tx pin high, send 1
			tx_mask = 0x01;

			tx_state = 1;
			break;

		case 1:
			if (tx_byte & tx_mask) // choose bit
				SET_BIT(PORTB, TXPIN); // tx pin high, send 1
			else
				CLEAR_BIT(PORTB, TXPIN); // tx pin low, send 0

			tx_mask <<= 1;

			if (tx_mask == 0)
			{
				tx_state = 2;
			}
			break;

		case 2:
			SET_BIT(PORTB, TXPIN); // tx pin high, send 1
			tx_state = 3;

			break;

		case 3:
			tx_state = 4;
			break;

		case 4:
			disable_serial();
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