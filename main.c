#include "led.h"
#include "power.h"

int main(int /*argc*/, char** /*argv*/)
{
	led_init();
	power_init();
	return 0;
}
