#include "adc.h"
#include "audio.h"
#include "fix_fft.h"
#include "led.h"

#include <string.h>

#define ARRAY_BITS 3
#define ARRAY_SIZE (1 << ARRAY_BITS)

static i8 s_audio_real[ARRAY_SIZE];
static i8 s_audio_imag[ARRAY_SIZE];
static volatile u8 s_audio_write_index;

void audio_init()
{
}

u8 amplitude_at(u8 index, i16 /*scale*/)
{
	i16 real = s_audio_real[index];
	i16 imag = s_audio_imag[index];

	i16 magnitude = (real * real) + (imag * imag);
	return magnitude / 256 / 2;
}

static u8 s_status = 0;
void audio_render_effects()
{
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
				amplitude_at(2, scale),
				amplitude_at(1, scale),
				amplitude_at(0, scale)
		);
		led_set2(
				s_status++,
				amplitude_at(3, scale),
				amplitude_at(4, scale)
		);
	}
}

void adc_on_read(u8 value)
{
	u8 index = s_audio_write_index;
	s_audio_real[index] = value;
	s_audio_write_index = index + 1;
}