#include "adc.h"
#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

typedef enum
{
	INPUT_BATTERY,
	INPUT_AUDIO_LEFT,
	INPUT_AUDIO_RIGHT,
} Input;

// Set to true if the sample should be discarded.
// Used when changing the MUX.
static bool s_discard = false;

// Type of ADC input that is being generated.
static Input s_input = INPUT_BATTERY;

void adc_init()
{
	// Disable digital input for Battery
	DIDR0 = MASK(ADC6D);
	// Disable digital input for Audio Left and Audio Right
	DIDR1 = MASK(ADC7D) | MASK(ADC8D);

	// Prepare for audio/battery reading.
	ADCSRA =
		MASK(ADIE) | // Enable ADC interrupt
		MASK(ADPS2) | MASK(ADPS1) | MASK(ADPS0) | // Set ADC prescaler to 128
		MASK(ADATE) | // ADC Auto Trigger
		MASK(ADEN) | // Enable ADC
		MASK(ADSC); // Start converting
}

void adc_read_battery()
{
	ADMUX = MASK(MUX1) | MASK(MUX2) | MASK(REFS2) | MASK(REFS1);
	s_discard = true;
	s_input = INPUT_BATTERY;
}

void adc_read_audio_left()
{
	ADMUX = MASK(MUX0) | MASK(MUX1) | MASK(MUX2) | MASK(ADLAR);
	s_discard = true;
	s_input = INPUT_AUDIO_LEFT;
}

void adc_stop()
{
	CLEAR_BIT(ADCSRA, ADEN);
}

ISR(ADC_vect)
{
	if (s_discard)
	{
		// Discard one sample
		(void) ADCH;
		s_discard = false;
	}
	else if (s_input == INPUT_BATTERY)
	{
		u16 voltage = ADCL;
		voltage |= ((u16) ADCH << 8);
		battery_on_read(ADCH);
	}
	else // s_input == INPUT_AUDIO_LEFT
	{
		audio_on_read_left(ADCH);
	}
}
