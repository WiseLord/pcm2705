#include "pins.h"

#include <util/delay.h>

void hwInit(void)
{
	DDR(PCM2705_MD) |= PCM2705_MD_LINE;
	DDR(PCM2705_MC) |= PCM2705_MC_LINE;
	DDR(PCM2705_MS) |= PCM2705_MS_LINE;

	PORT(PCM2705_MD) |= PCM2705_MD_LINE;
	PORT(PCM2705_MC) |= PCM2705_MC_LINE;
	PORT(PCM2705_MS) |= PCM2705_MS_LINE;

	return;
}

void pcm2705SendBit(uint8_t bit)
{
	PORT(PCM2705_MC) |= PCM2705_MC_LINE;
	_delay_us(1);
	if (bit) {
		PORT(PCM2705_MD) |= PCM2705_MD_LINE;
	} else {
		PORT(PCM2705_MD) &= ~PCM2705_MD_LINE;
	}
	_delay_us(1);
	PORT(PCM2705_MC) &= ~PCM2705_MC_LINE;
	_delay_us(2);
	PORT(PCM2705_MC) |= PCM2705_MC_LINE;
	_delay_us(1);

	return;
}

void pcm2705Send(uint8_t data)
{
	int8_t i;

	_delay_us(1);
	for (i = 7; i >= 0; i--)
		pcm2705SendBit(data & (1<<i));

	return;
}

int main(void)
{
	DDR(LED) |= LED_LINE;

	hwInit();

	PORT(PCM2705_MS) &= ~PCM2705_MS_LINE;
	_delay_us(1);
	pcm2705Send(0b00000000);
	pcm2705Send(0b00000000);
	_delay_us(1);
	PORT(PCM2705_MS) |= PCM2705_MS_LINE;

	while(1) {
		_delay_ms(4800);

		PORT(PCM2705_MS) &= ~PCM2705_MS_LINE;
		_delay_us(1);
		pcm2705Send(0b00000000);
		pcm2705Send(0b00000001);
		_delay_us(1);
		PORT(PCM2705_MS) |= PCM2705_MS_LINE;


		PORT(PCM2705_MS) &= ~PCM2705_MS_LINE;
		_delay_us(1);
		pcm2705Send(0b00000000);
		pcm2705Send(0b00000000);
		_delay_us(1);
		PORT(PCM2705_MS) |= PCM2705_MS_LINE;


		PORT(LED) |= LED_LINE;
		_delay_ms(200);
		PORT(LED) &= ~LED_LINE;
	}

	return 0;
}

