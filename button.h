/**@file
 * @brief
 */
#ifndef _PINS_H_
#define _PINS_H_

#include "defs.h"
#include <stdbool.h>

void enter_deepsleep();

void disable_on_interrupt();
void enable_on_interrupt();

void button_init(void);
bool button_is_pressed(void);
void on_button_interrupt();

void wakey_wakey();
void nap_time();
void button_menu();

extern volatile bool global_night_light_enable;
extern volatile bool global_only_music_enable;
extern volatile u16 night_light_counter;
extern u16 main_loop_counter;   // pseudo random source

#define MENU_NOTHING 0
#define MENU_SHUTDOWN 1
#define MENU_LIGHT_TOGGLE 2
#define MENU_MUSIC_TOGGLE 3

#endif /* _PINS_H_ */
