/**@file
 * @brief
 */
#ifndef _PINS_H_
#define _PINS_H_

#include "defs.h"
#include <stdbool.h>

typedef enum {
    // modus_nothing,
    modus_shutdown,
	modus_night_light,
    modus_music_only,
    modus_normal
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
