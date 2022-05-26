#include "battery.h"

#include "adc.h"
#include "led.h"

#define WAIT_READ 0xFFFFU

typedef enum
{
	BATT_GOOD,
	BATT_LOW,
	BATT_UNKNOWN
} BatteryState;

static volatile u8 s_status;

static BatteryState battery_get_status()
{
	while (s_status == BATT_UNKNOWN);
	return s_status;
}

void battery_init()
{
}

void battery_update()
{
	adc_read_battery();
	s_status = BATT_UNKNOWN;
}

bool battery_low()
{
	return battery_get_status();
}

void battery_on_read(u16 value)
{
	if (value < CENTI_VOLTS_TO_VALUE(420))
		s_status = BATT_LOW;
	else
		s_status = BATT_GOOD;
}
