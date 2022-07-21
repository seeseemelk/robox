/**@file
 * @brief
 */
#ifndef _PINS_H_
#define _PINS_H_

#include <stdbool.h>

void enter_deepsleep();

void disable_on_interrupt();
void enable_on_interrupt();

void button_init(void);
bool button_is_pressed(void);
void on_button_interrupt();

void wakey_wakey();
void nap_time();
void check_if_tired();

#endif /* _PINS_H_ */
