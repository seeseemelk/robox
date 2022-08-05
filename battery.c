#include "battery.h"

#include "adc.h"
#include "button.h"
#include "led.h"
#include "power.h"
#include <avr/interrupt.h>

#define WAIT_READ 0xFFFFU

static volatile u16 s_value;
static bool s_charging;
// static bool s_lowBattery;
static u16 s_debounce_bat_critical;
static u16 s_debounce_bat_low;

void battery_init()
{
	s_debounce_bat_critical = 0;
	// s_lowBattery = false;
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
		if (++s_debounce_bat_critical >= 0xFFF)
		{
			// s_lowBattery = true;
			cli();
			enter_deepsleep();
			sei();
			s_debounce_bat_critical = 0;
			return BATT_CRIT;
		}
	}
	else
		s_debounce_bat_critical = 0;

	if (power_is_psu_standby())
		return BATT_FULL;
	else if (s_charging)
		return BATT_CHARGING;
	// else if (s_lowBattery == false && s_value < CENTI_VOLTS_TO_VALUE(345))
	// {
	// 	// measure multiple times to reduce effect of spikes
	// 	if (s_debounce_bat_low >= 0xFFF)
	// 	{
	// 		s_lowBattery = true;
	// 		return BATT_LOW;
	// 	}
	// 	else
	// 		s_debounce_bat_low++;
	// }
	// else if (s_lowBattery == true && s_value < CENTI_VOLTS_TO_VALUE(370))
	else if (s_value < CENTI_VOLTS_TO_VALUE(345))
	{
		// measure multiple times to reduce effect of spikes
		if (s_debounce_bat_low >= 0xFFF)
		// {
			// s_lowBattery = true;
			return BATT_LOW;
		// }
		else
		{
			s_debounce_bat_low++;
			return BATT_GOOD;
		}
	}
	else
	{
		s_debounce_bat_low = 0;
		// s_lowBattery = false;
		return BATT_GOOD;
	}
}

void battery_on_read(u16 value)
{
	if (s_value == WAIT_READ)
		s_value = value;
}
