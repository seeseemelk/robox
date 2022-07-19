#include "battery.h"

#include "adc.h"
#include "button.h"
#include "led.h"
#include "power.h"

#define WAIT_READ 0xFFFFU

static volatile u16 s_value;
static bool s_charging;
static bool s_lowBattery;
static int s_deepSleepDebounce;

void battery_init()
{
	s_deepSleepDebounce = 0;
	s_lowBattery = false;
}

void battery_update()
{
	s_charging = power_is_psu_charging();
	s_value = WAIT_READ;
	adc_read_battery();
}

BatteryState battery_status()
{
	while (s_value == WAIT_READ);

	if (s_value < CENTI_VOLTS_TO_VALUE(320))
	{
		if (++s_deepSleepDebounce == 64)
		{
			s_lowBattery = true;
			enter_deepsleep();
			s_deepSleepDebounce = 0;
			return BATT_CRIT;
		}
	}
	else
		s_deepSleepDebounce = 0;

	if (power_is_psu_standby())
		return BATT_FULL;
	else if (s_charging)
		return BATT_CHARGING;
	else if (s_lowBattery == false && s_value < CENTI_VOLTS_TO_VALUE(345))
	{
		s_lowBattery = true;
		return BATT_LOW;
	}
	else if (s_lowBattery == true && s_value < CENTI_VOLTS_TO_VALUE(370))
	{
		s_lowBattery = true;
		return BATT_LOW;
	}
	else
	{
		s_lowBattery = false;
		return BATT_GOOD;
	}
}

void battery_on_read(u16 value)
{
	if (s_value == WAIT_READ)
		s_value = value;
}
