#ifndef _ADC_H_
#define _ADC_H_

#include "defs.h"

void adc_init();
void adc_stop();
void adc_on_read(u8 value);
void adc_read_audio_left();

#endif
