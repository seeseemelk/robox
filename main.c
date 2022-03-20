#include "adc.h"
#include "audio.h"
#include "fix_fft.h"
#include "led.h"
#include "power.h"

#include <avr/interrupt.h>

const i8 s_wave[] PROGMEM =
{
	1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

i8 waveAt(i8 point)
{
	i8 index = point % sizeof(s_wave);
	if (((point / sizeof(s_wave)) & 1) == 0)
		return pgm_read_byte_near(s_wave + index);
	else
		return pgm_read_byte_near(s_wave + sizeof(s_wave) - index - 1);
}

int main()//(int /*argc*/, char** /*argv*/)
{
	cli();
	led_init();
	power_init();
	adc_init();
	sei();

	u16 i = 0;
	while (1)
	{
		audio_render_effects();
		led_set2(
				waveAt(i + 0),
				waveAt(i + 3),
				waveAt(i + 6)
		);
		led_set1(
				waveAt(i + 9),
				waveAt(i + 12),
				waveAt(i + 16)
		);

		i = (i + 1) % (sizeof(s_wave) * 2);
//
//		for (u16 i = 0; i < sizeof(s_real); i++)
//			s_real[i] = i;
//		fix_fft(s_real, s_imaginary, FFT_BITS, false);
	}
}
