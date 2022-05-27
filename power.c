#include "power.h"

#include "config.h"
#include "defs.h"

#include <avr/io.h>

#define SHDWN_BLE PORTA3
#define PSU_CHG PORTB1
#define PSU_STBY PORTB3

void power_init()
{
	DDRA |= MASK(SHDWN_BLE);
}

bool power_is_psu_charging()
{
#ifdef INVERT_IO
	return TEST_BIT_CLEAR(PINB, PSU_CHG);
#else
	return TEST_BIT_SET(PINB, PSU_CHG);
#endif
}

bool power_is_psu_standby()
{
#ifdef INVERT_IO
	return TEST_BIT_CLEAR(PINB, PSU_STBY);
#else
	return TEST_BIT_SET(PINB, PSU_STBY);
#endif
}

void power_enable_ble()
{
	CLEAR_BIT(PORTA, SHDWN_BLE);
}

void power_disable_ble()
{
	SET_BIT(PORTA, SHDWN_BLE);
}
