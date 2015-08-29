#include "pcm2705.h"
#include "pins.h"

#include <util/delay.h>

void hwInit(void)
{
	pcm2705Init();

	return;
}

int main(void)
{
	DDR(LED) |= LED_LINE;

	hwInit();

	while(1) {
		_delay_ms(4800);

		pcm2705HidCmd(PCM2705_HID_NEXT_TRACK);

		PORT(LED) |= LED_LINE;
		_delay_ms(200);
		PORT(LED) &= ~LED_LINE;
	}

	return 0;
}

