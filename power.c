#include "power.h"
#include "defs.h"

#include <avr/io.h>

#define SHDWN_AMP 0
#define SHDWN_BLE 1
#define PSU_CHG 2
#define PSU_STBY 3

void power_init()
{
	DDRB |= MASK(SHDWN_AMP) | MASK(SHDWN_BLE);
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
	SET_BIT(PORTB, SHDWN_AMP);
}

void power_disable_amp()
{
	CLEAR_BIT(PORTB, SHDWN_AMP);
}

void power_enable_ble()
{
	SET_BIT(PORTB, SHDWN_AMP);
}

void power_disable_ble()
{
	CLEAR_BIT(PORTB, SHDWN_AMP);
}
