#ifndef BATTERY_H_
#define BATTERY_H_

#include <stdbool.h>

typedef enum
{
	BATT_GOOD,
	BATT_CHARGING,
	BATT_FULL,
	BATT_LOW,
	BATT_UNKNOWN
} BatteryState;

void battery_init();
void battery_update();
BatteryState battery_status();

#endif /* BATTERY_H_ */
