/**@file
 * @brief
 */
#ifndef _PINS_H_
#define _PINS_H_

#include "defs.h"
#include <stdbool.h>

typedef enum {
    // modus_nothing,
    press_long,
    press_1_short,
    press_2_short,
    press_nothing
} ButtonPressPattern;

typedef enum {
    // modus_nothing,
    mapper_shutdown,
	mapper_night_light,
	mapper_music_night_light,
    mapper_music_only,
    mapper_normal_mode
} GlobalModus;

void enter_deepsleep();

void button_init(void);
bool button_is_pressed(void);

void wakey_wakey();
void nap_time();
void button_menu();

extern volatile GlobalModus global_modus;
extern volatile u16 night_light_counter;
extern u16 main_loop_counter;   // pseudo random source

#endif /* _PINS_H_ */
