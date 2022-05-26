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

static volatile u8 s_read;

static BatteryState battery_wait_for_read()
{
	while (s_read == BATT_UNKNOWN);
	return s_read;
}

void battery_init()
{
}

void battery_update()
{
	adc_read_battery();
	s_read = BATT_UNKNOWN;
}

bool battery_low()
{
	return battery_wait_for_read();
}

void battery_on_read(u16 value)
{
	if (value < CENTI_VOLTS_TO_VALUE(420))
		s_read = BATT_LOW;
	else
		s_read = BATT_GOOD;

//	(void) value;
//	s_read = BATT_LOW;
//	led_set1((value / 64) & 0x3F, 0, 0);
}
