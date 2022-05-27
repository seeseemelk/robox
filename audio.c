#include "adc.h"
#include "audio.h"
#include "fix_fft.h"
#include "led.h"

#include <string.h>
#include <avr/interrupt.h>


#define ARRAY_BITS 3
#define ARRAY_SIZE (1 << ARRAY_BITS)

static i8 s_audio_real[ARRAY_SIZE];
static i8 s_audio_imag[ARRAY_SIZE];
static volatile u8 s_audio_write_index;

static bool min_amplitude = true;
static bool min_time_separation = true;
// static u8 previous_avg_beats = 0;
// static double leakey_avg_1 = 0;
// static double leakey_avg_all = 0;

static u8 max_previous = 20;
static u8 max_current = 0;

// static u8 amplitude_at_1_previous = 0;
// static u8 amplitude_at_2_previous = 0;
// static u8 amplitude_at_3_previous = 0;
// static u8 amplitude_at_4_previous = 0;
// static u8 amplitude_at_5_previous = 0;
// static u8 amplitude_at_6_previous = 0;

// static bool test_toggle = false;


void audio_init()
{
	// timer for beat detection 
	// to guarantee min time separation between beats of max 250 BPM
	// 250 BPM = 4ms

	// Configure Timer 1 to call the output-compare match interrupt
	// Enable Timer 1 output compare interrupt A
	SET_BIT(TIMSK, OCIE1A);
	// Configure the output compare register
	OCR1A = 80;
	// Disable pin output functions
	// TCCR1A &= ~(MASK(COM1A1) | MASK(COM1A0) | MASK(COM1B1) | MASK(COM1B0));
	// TCCR1C &= ~(MASK(COM1D1) | MASK(COM1D0))
	// TCCR1C = 0;	// also includes TCCR1A shadow bits
	// Prescaler: clkIO / 256
	TCCR1B = MASK(CS13) | MASK(CS12) | MASK(CS11) | MASK(CS10);
	// TCCR1B = MASK(CS13) | MASK(CS10);

}

ISR(TIMER1_COMPA_vect)
{
	// 4Hz loop
	TCNT1 = 0;

	min_time_separation = true;
	max_previous = max_current * 0.9;
	max_current = 0;
}

u8 amplitude_at(u8 index)//, i16 scale)
{
	i16 real = s_audio_real[index];
	i16 imag = s_audio_imag[index];

	i16 magnitude = (real * real) + (imag * imag);
	return magnitude / 256 / 4;
}

// static u8 s_status = 0;
void audio_render_effects()
{
	// double avg_beats = 0;
	u8 amplitude_at_1 = 0;

	adc_read_audio_left();
	s_audio_write_index = 0;
	memset(s_audio_imag, 0, ARRAY_SIZE);

	// Wait until conversion is ready.
	while (s_audio_write_index < ARRAY_SIZE) {}

	i16 scale = fix_fft(s_audio_real, s_audio_imag, ARRAY_BITS, false);
	if (scale == -1)
	{
		led_set1(0, 8, 0);
		return;
	}
	else
	{
		
		amplitude_at_1 = amplitude_at(1);

		if ((amplitude_at_1 > max_current) && (max_current != 0))
			max_current = amplitude_at_1;

		if (amplitude_at_1 == 0)
		{
			min_amplitude = true;
		}
		// for (u8 i = 1; i<ARRAY_SIZE; i++)
		// 	avg_beats = ((double)amplitude_at(i)) / ((double)ARRAY_SIZE);

		// leakey_avg_1 = (amplitude_at(1) * 0.125) + (leakey_avg_1 * 0.875);
		// leakey_avg_all = (avg_beats * 0.125) + (leakey_avg_all * 0.875);
		

		// if ((min_reached == false) && (leakey_avg_1 < 20))
		// {
		// 	min_reached = true;
		// }

		// 
		if ((min_time_separation == true) && (min_amplitude == true) && (max_previous <= amplitude_at_1) )
		{
			if (amplitude_at(1) || amplitude_at(2) || amplitude_at(3))
			{
				led_set1(
					amplitude_at(3),
					amplitude_at(2),
					amplitude_at(1)
				);
			}
			if (amplitude_at(4) || amplitude_at(5) || amplitude_at(6))
			{
				led_set2(
					amplitude_at(6),
					amplitude_at(5),
					amplitude_at(4)
				);
			}
			min_amplitude = false;
			min_time_separation = false;
		}
		// previous_avg_beats = avg_beats;

		// led_set1(
		// 		amplitude_at(1),
		// 		amplitude_at(1),
		// 		amplitude_at(1)
		// );
		// led_set2(
		// 		amplitude_at(4),
		// 		amplitude_at(3),
		// 		amplitude_at(2)
		// 		// amplitude_at(1)
		// );
	}
}

bool decimate = false;
void adc_on_read(u8 value)
{
	if (decimate)
	{
		u8 index = s_audio_write_index;
		if (index < ARRAY_SIZE)
		{
			s_audio_real[index] = value;
			s_audio_write_index = index + 1;
		}
	}

	decimate = !decimate;
}