#include "battery.h"

#include "adc.h"
#include "led.h"
#include "power.h"

#define WAIT_READ 0xFFFFU

static volatile u8 s_status;
static bool s_charging;

void battery_init()
{
}

void battery_update()
{
	s_charging = power_is_psu_charging();
	adc_read_battery();
	s_status = BATT_UNKNOWN;
}

BatteryState battery_status()
{
	while (s_status == BATT_UNKNOWN);
	return s_status;
}

void battery_on_read(u16 value)
{
	if (value < CENTI_VOLTS_TO_VALUE(350))
		s_status = BATT_LOW;
	else if (s_charging)
	{
		if (value > CENTI_VOLTS_TO_VALUE(400))
			s_status = BATT_FULL;
		else if (power_is_psu_charging())
			s_status = BATT_CHARGING;
	}
	else
		s_status = BATT_GOOD;
}
