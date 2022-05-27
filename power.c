#include "power.h"

#include "config.h"
#include "defs.h"

#include <avr/io.h>

#define SHDWN_BLE PORTA3
#define PSU_CHG PINB1
#define PSU_STBY PINB3

void power_init()
{
	DDRA |= MASK(SHDWN_BLE);
}

bool power_is_psu_charging()
{
	return TEST_BIT_CLEAR(PINB, PSU_CHG);
}

bool power_is_psu_standby()
{
	return TEST_BIT_CLEAR(PINB, PSU_STBY);
}

void power_enable_ble()
{
	CLEAR_BIT(PORTA, SHDWN_BLE);
}

void power_disable_ble()
{
	SET_BIT(PORTA, SHDWN_BLE);
}
