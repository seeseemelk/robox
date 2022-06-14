#include "adc.h"
#include "defs.h"

#include "led.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

typedef enum
{
	INPUT_BATTERY,
	INPUT_BATTERY_DISCARD,
	INPUT_AUDIO_LEFT,
	INPUT_AUDIO_LEFT_DISCARD,
	INPUT_NONE,
} Input;

/*
 * Some defines to set or clear bits in ADCSRA.
 * Why?
 * The ADIF bit is set to zero when you write a one to it. This can
 * potential cause the loss of an interrupt.
 * I wish Atmel had put that flag in a different register...
 */
#define ADCSRA_CLEAR(bit) ADCSRA = (ADCSRA & ~(MASK(ADIF) | MASK(bit)))
#define ADCSRA_SET(bit) ADCSRA = (ADCSRA & ~MASK(ADIF)) | MASK(bit)

// Type of ADC input that is being generated.
static Input s_input = INPUT_NONE;

// Discard the next value that is read.
//static Discard s_discard = DISCARD_ALL;

void adc_init()
{
	// Disable digital input for Battery
	DIDR0 = MASK(ADC6D);
	// Disable digital input for Audio Left and Audio Right
	DIDR1 = MASK(ADC7D) | MASK(ADC8D);

	// Prepare for audio/battery reading.
	ADCSRA =
		MASK(ADEN) | // Enable ADC
		MASK(ADIE) | // Enable ADC interrupt
		MASK(ADPS1) | MASK(ADPS0) | // Set ADC prescaler to 8
		MASK(ADATE) | // ADC Auto Trigger
		MASK(ADSC); // Start converting
}

void adc_read_battery()
{
	if (s_input != INPUT_BATTERY)
	{
		s_input = INPUT_NONE;
		ADMUX = MASK(MUX1) | MASK(MUX2) | MASK(REFS1);
		ADCSRB |= MASK(REFS2);
		s_input = INPUT_BATTERY_DISCARD;
	}
}

void adc_read_audio_left()
{
	if (s_input != INPUT_AUDIO_LEFT)
	{
		s_input = INPUT_NONE;
		ADMUX = MASK(MUX0) | MASK(MUX1) | MASK(MUX2) | MASK(ADLAR);
		ADCSRB &= ~MASK(REFS2);
		s_input = INPUT_AUDIO_LEFT_DISCARD;
	}
}

void adc_stop()
{
	ADCSRA_CLEAR(ADEN);
}

ISR(ADC_vect)
{
	u16 voltage;
	switch (s_input)
	{
	case INPUT_BATTERY_DISCARD:
		(void) ADCH;
		s_input = INPUT_BATTERY;
		break;
	case INPUT_AUDIO_LEFT_DISCARD:
		(void) ADCH;
		s_input = INPUT_AUDIO_LEFT;
		break;
	case INPUT_BATTERY:
		voltage = ADCL;
		voltage |= (((u16) ADCH) << 8);
		battery_on_read(voltage);
		break;
	case INPUT_AUDIO_LEFT:
		audio_on_read_left(ADCH);
		break;
	case INPUT_NONE:
		break;
	}
}
