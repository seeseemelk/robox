#include "battery.h"

#include "adc.h"
#include "button.h"
#include "led.h"
#include "power.h"

#define WAIT_READ 0xFFFFU

static volatile u16 s_value;
static bool s_charging;

void battery_init()
{
}

void battery_update()
{
	s_charging = power_is_psu_charging();
	adc_read_battery();
	s_value = WAIT_READ;
}

BatteryState battery_status()
{
	while (s_value == WAIT_READ);

	if (s_value < CENTI_VOLTS_TO_VALUE(320))
	{
		enter_deepsleep();
		return BATT_CRIT;
	}
	else if (s_charging && s_value > CENTI_VOLTS_TO_VALUE(400))
		return BATT_FULL;
	else if (s_charging)
		return BATT_CHARGING;
	else if (s_value < CENTI_VOLTS_TO_VALUE(350))
		return BATT_LOW;
	else
		return BATT_GOOD;
}

void battery_on_read(u16 value)
{
	s_value = value;
}
