#include "battery.h"

#include "adc.h"

static u16 s_read = 0;
static bool s_low = false;

static u16 battery_read()
{
	s_read = 0;
	adc_read_battery();
	while (s_read == 0) {}
	return s_read;
}

void battery_init()
{
}

void battery_update()
{
	adc_read_battery();
	u16 battery = battery_read();
	s_low = battery < CENTI_VOLTS_TO_VALUE(210);
}

bool battery_low()
{
	return s_low;
}

void battery_on_read(u16 value)
{
	s_read = value;
}
