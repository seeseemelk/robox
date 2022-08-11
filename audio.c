#include "adc.h"

#include "audio.h"
#include "battery.h"
#include "fix_fft.h"
#include "led.h"
#include "timer1.h"
#include "button.h"

#include <string.h>
#include <avr/interrupt.h>


#define ARRAY_BITS 3
#define ARRAY_SIZE (1 << ARRAY_BITS)

static i8 s_audio_real[ARRAY_SIZE];
static i8 s_audio_imag[ARRAY_SIZE];
static volatile u8 s_audio_write_index;

static bool min_amplitude = true;
static bool min_time_separation = true;

static u8 max_previous = 20;
static u8 max_current = 0;
// static u8 state_previous = BATT_UNKNOWN;

static volatile i16 color_buffer [6];

const i8 s_breath[] PROGMEM =
{
	0,  0,  0,  0,  0,  0,  0,
	1,  1,  2,  3,  4,  5,  6,  7,  8,
	12, 16, 20, 24, 28, 32, 36, 40, 48, 52, 58, 63,
};

static i8 breathing_at(i8 point)
{
	i8 index = point % sizeof(s_breath);
	if (((point / sizeof(s_breath)) & 1) == 0)
		return pgm_read_byte_near(s_breath + index);
	else
		return pgm_read_byte_near(s_breath + sizeof(s_breath) - index - 1);
}
static u16 s_breathing_index = 0;
static u16 s_skip = 0;

void audio_init()
{
	// timer for beat detection 
	// to guarantee min time separation between beats of max 250 BPM
	// 250 BPM = 4ms

	// Configure Timer 1 to call the output-compare match interrupt
	// Enable Timer 1 output compare interrupt A
	// SET_BIT(TIMSK, OCIE1A);
	
}

u8 amplitude_at(u8 index)
{
	i16 real = s_audio_real[index];
	i16 imag = s_audio_imag[index];

	i16 magnitude = (real * real) + (imag * imag);
	return magnitude / 256 / 4;
}

void copy_amplitude()
{
	for (u8 i=0; i<6; i++)
		color_buffer[i] = amplitude_at(i+1);
}

static void render_battery_effect(u8 maskR, u8 maskG, u8 maskB)
{
	if (s_skip == 0)
	{
		led_disable_scaling();
		i8 breathingA = breathing_at(s_breathing_index);
		i8 breathingB = breathing_at(s_breathing_index + sizeof(s_breath));
		s_breathing_index = (s_breathing_index + 1) % (sizeof(s_breath) * 2);

		led_set(&led1, breathingA & maskR, breathingA & maskG, breathingA & maskB, true);
		led_set(&led2, breathingB & maskR, breathingB & maskG, breathingB & maskB, true);
		led_enable_scaling();
	}
	s_skip = (s_skip + 1) % 512;
}

static u8 distance(u8 a, u8 b)
{
	if (a > b)
		return a - b;
	else
		return b - a;
}

void audio_render_effects()
{
	u8 state = battery_status();

	switch (state)
	{
	case BATT_LOW:
	case BATT_CRIT:
		render_battery_effect(0xFF, 0x00, 0x00);
		break;
	case BATT_CHARGING:
		render_battery_effect(0xFF, 0xFF, 0x00);
		break;
	case BATT_FULL:
	case BATT_GOOD:
	case BATT_UNKNOWN:
		// if ((state_previous == BATT_LOW) || (state_previous == BATT_CRIT) || (state_previous == BATT_CHARGING))
		// {
		// 	s_breathing_index = 0;
		// 	led_set_full(0x00);
		// }
		if (global_modus == mapper_normal_mode)
		{
			adc_read_audio_left();
			s_audio_write_index = 0;

			// Wait until conversion is ready.
			while (s_audio_write_index < ARRAY_SIZE);

			i16 scale = fix_fft(s_audio_real, s_audio_imag, ARRAY_BITS, false);
			if (scale == -1)
				led_set(&led1, 0, 8, 0, true);
			else
			{

				u8 amplitude_at_1 = amplitude_at(1);

				if ((amplitude_at_1 > max_current) && (max_current != 0))
					max_current = amplitude_at_1;

				if (amplitude_at_1 == 0)
					min_amplitude = true;

				if ((min_time_separation == true) && distance(max_previous, amplitude_at_1) > 4 )
				{
					led_enable_scaling();
					led_set(&led1, color_buffer[0], color_buffer[1], color_buffer[2], false);
					led_set(&led2, color_buffer[3], color_buffer[4], color_buffer[5], false);
					min_amplitude = false;
					min_time_separation = false;
				}
			}
		}
		else if ((global_modus == mapper_night_light) || (global_modus == mapper_music_rgb) || (global_modus == mapper_music_night_light))
			showRGB();
		else
			led_set_full(0x00);
		break;
	}

	// state_previous = state;
}

void audio_on_read_left(u8 value)
{
	u8 index = s_audio_write_index;
	if (index < ARRAY_SIZE)
	{
		s_audio_real[index] = value;
		s_audio_write_index = index + 1;
	}
}

ISR(TIMER1_COMPA_vect)
{
	// 4Hz loop
	TCNT1 = 0;

	min_time_separation = true;
	max_previous = max_current * 0.9;
	max_current = 0;

	copy_amplitude();
}