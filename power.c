#include "power.h"
#include "defs.h"

#include <avr/io.h>

#define SHDWN_AMP PORTA3
#define SHDWN_BLE PORTA3
#define PSU_CHG PORTB2
#define PSU_STBY PORTB3

void power_init()
{
	DDRA |= MASK(SHDWN_AMP) | MASK(SHDWN_BLE);
}

bool power_is_psu_charging()
{
	return bit_is_set(PINB, PSU_CHG) != 0;
}

bool power_is_psu_standby()
{
	return TEST_BIT_SET(PINB, PSU_CHG);
}

void power_enable_amp()
{
	CLEAR_BIT(PORTA, SHDWN_AMP);
}

void power_disable_amp()
{
	SET_BIT(PORTA, SHDWN_AMP);
}

void power_enable_ble()
{
	CLEAR_BIT(PORTA, SHDWN_AMP);
}

void power_disable_ble()
{
	SET_BIT(PORTA, SHDWN_AMP);
}