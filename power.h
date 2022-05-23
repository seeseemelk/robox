#ifndef _POWER_H_
#define _POWER_H_

#include <stdbool.h>

void power_init();
bool power_is_psu_charging();
bool power_is_psu_standby();

void power_disable_ble();
void power_enable_ble();

void power_disable_amp();
void power_enable_amp();

#endif
