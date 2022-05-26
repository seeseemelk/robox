/**@file
 * @brief
 */
#ifndef _PINS_H_
#define _PINS_H_

#include <stdbool.h>

extern bool powerState;

void button_init(void);
bool button_is_pressed(void);
void on_button_interrupt();

void wakey_wakey();
void nap_time();

#endif /* _PINS_H_ */
