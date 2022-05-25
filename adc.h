#ifndef _ADC_H_
#define _ADC_H_

#include "defs.h"

#define CENTI_VOLTS_TO_VALUE(v) (v >> 3)

void adc_init();
void adc_stop();
void adc_on_read(u8 value);
void adc_read_audio_left();
void adc_read_battery();
void battery_on_read(u16 value);
void audio_on_read_left();

#endif
