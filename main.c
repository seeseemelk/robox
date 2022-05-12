#include "adc.h"
#include "audio.h"
#include "button.h"
#include "fix_fft.h"
#include "led.h"
#include "power.h"

#include <avr/interrupt.h>
#include <util/delay.h>

const i8 s_wave[] PROGMEM =
{
	//1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
	 0,  0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,  0,
	 1,  1,  2,  4,  8, 16, 32, 48, 63,
};
#define WAVE_OFFSET (sizeof(s_wave) / 6)

i8 waveAt(i8 point)
{
	i8 index = point % sizeof(s_wave);
	if (((point / sizeof(s_wave)) & 1) == 0)
		return pgm_read_byte_near(s_wave + index);
	else
		return pgm_read_byte_near(s_wave + sizeof(s_wave) - index - 1);
}

int main(int /*argc*/, char** /*argv*/)
{
	cli();
	button_init();
	led_init();
	power_init();
	adc_init();
	sei();

	u16 i = 0;
	while (1)
	{
		if (button_is_pressed())
		{
	//		audio_render_effects();
			led_set2(
					waveAt(i + WAVE_OFFSET * 0),
					waveAt(i + WAVE_OFFSET * 1),
					waveAt(i + WAVE_OFFSET * 2)
			);
			led_set1(
					waveAt(i + WAVE_OFFSET * 3),
					waveAt(i + WAVE_OFFSET * 4),
					waveAt(i + WAVE_OFFSET * 5)
			);

			i = (i + 1) % (sizeof(s_wave) * 2);
			_delay_ms(10);

	//		for (u16 i = 0; i < sizeof(s_real); i++)
	//			s_real[i] = i;
	//		fix_fft(s_real, s_imaginary, FFT_BITS, false);
		}
	}
}
