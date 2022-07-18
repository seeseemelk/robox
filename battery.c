#include "battery.h"

#include "adc.h"
#include "button.h"
#include "led.h"
#include "power.h"

#define WAIT_READ 0xFFFFU

static volatile u16 s_value;
static bool s_charging;
static BatteryState s_lastState;
static BatteryState s_newState;
static int s_stateDebounce;

void battery_init()
{
	s_stateDebounce = 0;
	s_newState = BATT_UNKNOWN;
}

void battery_update()
{
	s_charging = power_is_psu_charging();
	adc_read_battery();
	s_value = WAIT_READ;
	s_lastState = BATT_UNKNOWN;
}

static BatteryState calculate_battery_status()
{
	while (s_value == WAIT_READ);

	if (s_value < CENTI_VOLTS_TO_VALUE(320))
	{
		enter_deepsleep();
		return BATT_CRIT;
	}
	else if (power_is_psu_standby())
		return BATT_FULL;
	else if (s_charging)
		return BATT_CHARGING;
	else if (s_value < CENTI_VOLTS_TO_VALUE(345))
	{
		return BATT_LOW;
	}
	else if (s_lastState == BATT_LOW && s_value < CENTI_VOLTS_TO_VALUE(365))
	{
		return BATT_LOW;
	}
	else
	{
		return BATT_GOOD;
	}
}

BatteryState battery_status()
{
	BatteryState state = calculate_battery_status();
	if (state == s_newState)
	{
		if (s_stateDebounce == 0)
			s_lastState = state;
		else
			s_stateDebounce--;
	}
	else
	{
		s_newState = state;
		s_stateDebounce = 2;
	}
	return s_lastState;
}

void battery_on_read(u16 value)
{
	s_value = value;
}
