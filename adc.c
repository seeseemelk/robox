#include "adc.h"
#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>

void adc_init()
{
	// Disable digital input for Audio Left
	DIDR1 = MASK(ADC7D);

	ADCSRA =
		MASK(ADIE) | // Enable ADC interrupt
		MASK(ADPS2) | MASK(ADPS1) | MASK(ADPS0) | // Set ADC prescaler to 128
		MASK(ADLAR) | // Left-adjust values as we only need 8 out of 10 bits
		MASK(ADEN) | // Enable ADC
		MASK(ADSC); // Start converting

	// Start conversion
//	SET_BIT(ADCSRA, ADSC);
}

void adc_read_audio_left()
{
	ADMUX = MASK(MUX0) | MASK(MUX1) | MASK(MUX2) | MASK(ADLAR);
}

void adc_stop()
{
	CLEAR_BIT(ADCSRA, ADEN);
}

ISR(ADC_vect)
{
	adc_on_read(ADCH);
}
