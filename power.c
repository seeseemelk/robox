#include "power.h"
#include "defs.h"

#include <avr/io.h>

#define SHDWN_BLE 3
#define PSU_CHG 1
#define PSU_STBY 3

void power_init()
{
	DDRA |= MASK(SHDWN_BLE);
}

bool power_is_psu_charging()
{
	return bit_is_set(PINB, PSU_CHG) != 0;
}

bool power_is_psu_standby()
{
	return TEST_BIT_SET(PINB, PSU_CHG);
}

void power_enable_ble()
{
	CLEAR_BIT(PORTA, SHDWN_BLE);
}

void power_disable_ble()
{
	SET_BIT(PORTA, SHDWN_BLE);
}
