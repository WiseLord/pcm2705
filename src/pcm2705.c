#include "pcm2705.h"

#include "pins.h"

static void pcm2705SendBit(uint8_t bit)
{
	/* Set bit */
	if (bit)
		PORT(PCM2705_MD) |= PCM2705_MD_LINE;
	else
		PORT(PCM2705_MD) &= ~PCM2705_MD_LINE;
	/* Make strob */
	PORT(PCM2705_MC) |= PCM2705_MC_LINE;
	asm("nop");
	PORT(PCM2705_MC) &= ~PCM2705_MC_LINE;

	return;
}

static void pcm2705SendHid(uint16_t data)
{
	int8_t i;

	PORT(PCM2705_MS) &= ~PCM2705_MS_LINE;
	for (i = 15; i >= 0; i--)
		pcm2705SendBit(data & (1<<i));
	PORT(PCM2705_MS) |= PCM2705_MS_LINE;

	return;
}

void pcm2705Init()
{
	DDR(PCM2705_MD) |= PCM2705_MD_LINE;
	DDR(PCM2705_MC) |= PCM2705_MC_LINE;
	DDR(PCM2705_MS) |= PCM2705_MS_LINE;

	PORT(PCM2705_MD) |= PCM2705_MD_LINE;
	PORT(PCM2705_MC) &= ~PCM2705_MC_LINE;
	PORT(PCM2705_MS) |= PCM2705_MS_LINE;

	DDR(PCM2705_DPLUS) |= PCM2705_DPLUS_LINE;
	PORT(PCM2705_DPLUS) |= PCM2705_DPLUS_LINE;

}

void pcm2705HidCmd(uint8_t cmd)
{
	pcm2705SendHid(cmd);
	pcm2705SendHid(PCM2705_HID_NO_CMD);

	return;
}
