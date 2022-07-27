#ifndef _TIMER1_H_
#define _TIMER1_H_

#include <stdbool.h>
#include "defs.h"

#define WAIT_1S 40
#define WAIT_15M 36000
#define WAIT_100MS 4

// extern u8 timer_purpose;
extern volatile u16 counter_25ms;

// bool timer1_in_use();
void setup_button_menu();
void setup_beat_detection_counter();
void setup_25ms_interrupt();

void disable_timer1();

#endif