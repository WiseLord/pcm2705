#include <util/delay.h>
#include <avr/interrupt.h>

#include "pins.h"
#include "pcm2705.h"
#include "input.h"
#include "rc5.h"

void hwInit(void)
{
	pcm2705Init();
	inputInit();
	rc5Init();

	sei();

	return;
}

int main(void)
{
	uint8_t cmd;

	hwInit();

	while(1) {

		cmd = getBtnCmd();

		/* Flash led if button pressed */
		if (cmd != CMD_RC5_END)
			ledFlash(50);

		switch (cmd) {
		case CMD_RC5_MUTE:
		case CMD_BTN_MUTE:
			pcm2705HidCmd(PCM2705_HID_MUTE);
			break;
		case CMD_RC5_VOLUP:
		case CMD_BTN_VOLUP:
			pcm2705HidCmd(PCM2705_HID_VOL_UP);
			break;
		case CMD_RC5_VOLDN:
		case CMD_BTN_VOLDN:
			pcm2705HidCmd(PCM2705_HID_VOL_DOWN);
			break;
		case CMD_RC5_NEXT:
		case CMD_BTN_NEXT:
			pcm2705HidCmd(PCM2705_HID_NEXT_TRACK);
			break;
		case CMD_RC5_PREV:
		case CMD_BTN_PREV:
			pcm2705HidCmd(PCM2705_HID_PREV_TRACK);
			break;
		case CMD_RC5_STOP:
		case CMD_BTN_STOP:
			pcm2705HidCmd(PCM2705_HID_STOP);
			break;
		case CMD_RC5_PLAY:
		case CMD_BTN_PLAY:
			pcm2705HidCmd(PCM2705_HID_PLAY_PAUSE);
			break;
		default:
			break;
		}

	}

	return 0;
}
