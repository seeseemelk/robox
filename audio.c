#include "adc.h"

#include "audio.h"
#include "battery.h"
#include "fix_fft.h"
#include "led.h"

#include <string.h>

#define ARRAY_BITS 3
#define ARRAY_SIZE (1 << ARRAY_BITS)

static i8 s_audio_real[ARRAY_SIZE];
static i8 s_audio_imag[ARRAY_SIZE];
static volatile u8 s_audio_write_index;

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
}

u8 amplitude_at(u8 index)
{
	i16 real = s_audio_real[index];
	i16 imag = s_audio_imag[index];

	i16 magnitude = (real * real) + (imag * imag);
	return magnitude / 256 / 4;
}

// static u8 s_status = 0;
void audio_render_effects()
{
	switch (battery_status())
	{
	case BATT_LOW:
		if (s_skip == 0)
		{
			i8 breathingA = breathing_at(s_breathing_index);
			i8 breathingB = breathing_at(s_breathing_index + sizeof(s_breath));
			s_breathing_index = (s_breathing_index + 1) % (sizeof(s_breath) * 2);

			led_set1(breathingA, 0, 0);
			led_set2(breathingB, 0, 0);
		}
		s_skip = (s_skip + 1) % 16;
		break;
	case BATT_CHARGING:
		if (s_skip == 0)
		{
			i8 breathingA = breathing_at(s_breathing_index);
			i8 breathingB = breathing_at(s_breathing_index + sizeof(s_breath));
			s_breathing_index = (s_breathing_index + 1) % (sizeof(s_breath) * 2);

			led_set1(breathingA, breathingA, 0);
			led_set2(breathingB, breathingB, 0);
		}
		s_skip = (s_skip + 1) % 16;
		break;
	case BATT_FULL:
		if (s_skip == 0)
		{
			i8 breathingA = breathing_at(s_breathing_index);
			i8 breathingB = breathing_at(s_breathing_index + sizeof(s_breath));
			s_breathing_index = (s_breathing_index + 1) % (sizeof(s_breath) * 2);

			led_set1(0, breathingA, 0);
			led_set2(0, breathingB, 0);
		}
		s_skip = (s_skip + 1) % 16;
		break;
	case BATT_GOOD:
	case BATT_UNKNOWN:
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
			led_set1(
					amplitude_at(3),
					amplitude_at(2),
					amplitude_at(1)
			);
			led_set2(
					amplitude_at(6),
					amplitude_at(5),
					amplitude_at(4)
			);
		}
	}
}

bool decimate = false;
void audio_on_read_left(u8 value)
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
