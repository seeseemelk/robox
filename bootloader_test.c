#include "bootloader_test.h"

#include "defs.h"

#include <avr/io.h>
#include <stdbool.h>

#define LED1_R 2
#define LED1_G 1
#define LED1_B 0

#define LED2_R 6
#define LED2_G 5
#define LED2_B 4

#define OFFSET 126
#define RANGE 16

#define PIN_MASKS (MASK(LED1_R) | MASK(LED1_G) | MASK(LED1_B) \
	     | MASK(LED2_R) | MASK(LED2_G) | MASK(LED2_B))

#define BYTES_PER_PAGE 8
#define BYTES_PER_BLOCK (BYTES_PER_PAGE + 1)

static void select_left()
{
	ADMUX =
		( MASK(ADLAR) // Left adjust result
		| MASK(MUX0) | MASK(MUX1) | MASK(MUX2) // Select PB4
		);
}

static void select_right()
{
	ADMUX =
		( MASK(ADLAR) // Left adjust result
		| MASK(MUX3) // Select PA5
		);
}

static void start_conversion()
{
	ADCSRA |= MASK(ADSC);
}

static void wait_for_conversion()
{
	while (TEST_BIT_SET(ADCSRA, ADSC)) {}
}

static u8 read_adc()
{
	start_conversion();
	wait_for_conversion();
	return ADCH;
}

static i8 read_adc_left()
{
	select_left();
	i16 value = (u16) read_adc();
	return (i8) (value - OFFSET);
}

static i8 read_adc_right()
{
	select_right();
	i16 value = read_adc();
	return (i8) (value - OFFSET);
}

static i8 wait_for_clock_rising()
{
	u8 high_count = 0;
	for (;;)
	{
		i8 clock = read_adc_right();
		if (clock >= RANGE)
			high_count++;
		else
			high_count = 0;
		if (high_count > 0)
			return clock;
	}
}

static i8 wait_for_clock_falling()
{
	u8 low_count = 0;
	for (;;)
	{
		i8 clock = read_adc_right();
		if (clock < -(RANGE / 2))
			low_count++;
		else
			low_count = 0;
		if (low_count > 0)
			return clock;
	}
}

void bootloader_test()
{
	// Configure the LEDs as outputs
	DDRA |= PIN_MASKS;
	// Turns the LEDs off
	PORTA |= PIN_MASKS;
	PORTA &= ~MASK(LED2_G) & ~MASK(LED2_B);

	ADCSRA =
		( MASK(ADEN) // Enable ADC
		//| MASK(ADPS2) | MASK(ADPS1) | MASK(ADPS0) // 128 prescaler
		);

	u8 buffer[BYTES_PER_BLOCK];
	u8 byteIndex = 0;
	u8 bitIndex = 0;

	u8 correct = 2;
	while (1)
	{
		PORTA |= MASK(LED2_R);
		i8 clock = wait_for_clock_rising();
		i8 data = read_adc_left();
		PORTA &= ~MASK(LED2_R);

		u8 output = PIN_MASKS;
		if (data > 0)
		{
			buffer[byteIndex] = (buffer[byteIndex] >> 1) | 0x80;
			output &= ~MASK(LED1_B);
		}
		else if (data < 0)
		{
			buffer[byteIndex] = (buffer[byteIndex] >> 1) | 0x00;
			output &= ~MASK(LED1_G);
		}
		else
		{
			output &= ~MASK(LED1_R);
		}

		if (bitIndex == 7)
		{
			byteIndex++;
			bitIndex = 0;
		}
		else
		{
			bitIndex++;
		}

		if (byteIndex == BYTES_PER_BLOCK)
		{
			u8 sum = 0;
			for (u8 i = 0; i < BYTES_PER_PAGE; i++)
			{
				sum += buffer[i];
			}

			byteIndex = 0;
			bitIndex = 0;

			if ((sum ^ 0x55) != buffer[BYTES_PER_PAGE])
				correct = 0;
			else if (correct != 0)
				correct = 1;
		}

		if (correct == 1)
			output &= ~MASK(LED2_G);
		else if (correct == 0)
			output &= ~MASK(LED2_B);
		PORTA = output;

		//wait_for_clock_falling();
	}
}
