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
static bool s_swap = false;

void audio_init()
{
	// timer for beat detection 
	// to guarantee min time separation between beats of max 250 BPM
	// 250 BPM = 4ms

	// Configure Timer 1 to call the output-compare match interrupt
	// Enable Timer 1 output compare interrupt A
	// SET_BIT(TIMSK, OCIE1A);
	
}

ISR(TIMER1_COMPA_vect)
{
	// 4Hz loop
	TCNT1 = 0;

	min_time_separation = true;
	max_previous = max_current * 0.9;
	max_current = 0;
}

u8 amplitude_at(u8 index)
{
	i16 real = s_audio_real[index];
	i16 imag = s_audio_imag[index];

	i16 magnitude = (real * real) + (imag * imag);
	return magnitude / 256 / 4;
}

static void render_battery_effect(u8 maskR, u8 maskG, u8 maskB)
{
	if (s_skip == 0)
	{
		led_disable_scaling();
		i8 breathingA = breathing_at(s_breathing_index);
		i8 breathingB = breathing_at(s_breathing_index + sizeof(s_breath));
		s_breathing_index = (s_breathing_index + 1) % (sizeof(s_breath) * 2);

		led_set1(breathingA & maskR, breathingA & maskG, breathingA & maskB);
		led_set2(breathingB & maskR, breathingB & maskG, breathingB & maskB);
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

static void set_led(bool left, u8 r, u8 g, u8 b)
{
	if (left != s_swap)
		led_set1(r, g, b);
	else
		led_set2(r, g, b);
}

// static u8 s_status = 0;
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
		render_battery_effect(0x00, 0xFF, 0x00);
		break;
	case BATT_GOOD:
	case BATT_UNKNOWN:
		if (global_modus == mapper_normal_mode)
		{
			// u8 amplitude_at_1 = 0;

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

				u8 amplitude_at_1 = amplitude_at(1);

				if ((amplitude_at_1 > max_current) && (max_current != 0))
					max_current = amplitude_at_1;

				if (amplitude_at_1 == 0)
					min_amplitude = true;

				if ((min_time_separation == true) && distance(max_previous, amplitude_at_1) > 4 )
				{
					s_swap = !s_swap;
					if (amplitude_at(1) || amplitude_at(2) || amplitude_at(3))
					{
						set_led(true,
							amplitude_at(3),
							amplitude_at(2),
							amplitude_at(1)
						);
					}
					if (amplitude_at(4) || amplitude_at(5) || amplitude_at(6))
					{
						set_led(false,
							amplitude_at(6),
							amplitude_at(5),
							amplitude_at(4)
						);
					}
					min_amplitude = false;
					min_time_separation = false;
				}
			}
		}
		else if ((global_modus == mapper_night_light) || (global_modus == mapper_music_night_light))
			showRGB();
		break;
	}
}

static bool s_decimate = false;
void audio_on_read_left(u8 value)
{
	if (s_decimate)
	{
		u8 index = s_audio_write_index;
		if (index < ARRAY_SIZE)
		{
			s_audio_real[index] = value;
			s_audio_write_index = index + 1;
		}
	}

	s_decimate = !s_decimate;
}
