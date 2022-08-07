#include "battery.h"

#include "adc.h"
#include "button.h"
#include "led.h"
#include "power.h"
#include <avr/interrupt.h>

#define WAIT_READ 0xFFFFU

static volatile u16 s_value;
static bool s_debounce_bat_critical = false;
static bool s_debounce_bat_low = false;

void battery_init()
{
	s_debounce_bat_critical = 0;
}

void battery_update()
{
	s_value = WAIT_READ;
	adc_read_battery();
}

BatteryState battery_status()
{
	while (s_value == WAIT_READ);

	if (power_is_psu_standby())
	{
		s_debounce_bat_critical = false;
		s_debounce_bat_low = false;
		return BATT_FULL;
	}
	else if (power_is_psu_charging())
	{
		s_debounce_bat_critical = false;
		s_debounce_bat_low = false;
		return BATT_CHARGING;
	}
	else if (s_value < CENTI_VOLTS_TO_VALUE(320))
	{
		s_debounce_bat_low = false;

		// debounce provides 30s timeout
		if (s_debounce_bat_critical == false)
		{
			s_debounce_bat_critical = true;
			ticks_20ms = 0;
		}

		if (ticks_20ms >= TICKS_20MS_30S)
		{
			ticks_20ms = TICKS_20MS_30S;
			cli();
			enter_deepsleep();
			sei();
			s_debounce_bat_critical = false;
			return BATT_CRIT;
		}
	}
	else if (s_value < CENTI_VOLTS_TO_VALUE(345))
	{
		s_debounce_bat_critical = false;
		// debounce provides 30s timeout
		if (s_debounce_bat_low == false)
		{
			s_debounce_bat_low = true;
			ticks_20ms = 0;
		}

		if (ticks_20ms >= TICKS_20MS_30S)
		{
			ticks_20ms = TICKS_20MS_30S;
			return BATT_LOW;
		}
	}
	else
	{
		s_debounce_bat_critical = false;
		s_debounce_bat_low = false;
	}

	return BATT_GOOD;
}

void battery_on_read(u16 value)
{
	if (s_value == WAIT_READ)
		s_value = value;
}
