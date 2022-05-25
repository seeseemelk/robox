#ifndef BATTERY_H_
#define BATTERY_H_

#include <stdbool.h>

void battery_init();
void battery_update();
bool battery_low();

#endif /* BATTERY_H_ */
