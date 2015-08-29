#include <util/delay.h>
#include <avr/interrupt.h>

#include "pins.h"
#include "pcm2705.h"
#include "input.h"

void hwInit(void)
{
	pcm2705Init();
	inputInit();

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
		if (cmd != CMD_EMPTY)
			ledFlash(50);

		switch (cmd) {
		case CMD_RC5_MUTE:
		case CMD_BTN_0:
			pcm2705HidCmd(PCM2705_HID_MUTE);
			break;
		case CMD_RC5_VOL_UP:
		case CMD_BTN_1:
		case CMD_BTN_1_REPEAT:
			pcm2705HidCmd(PCM2705_HID_VOL_UP);
			break;
		case CMD_RC5_VOL_DOWN:
		case CMD_BTN_2:
		case CMD_BTN_2_REPEAT:
			pcm2705HidCmd(PCM2705_HID_VOL_DOWN);
			break;
		case CMD_RC5_NEXT_TRACK:
		case CMD_BTN_3:
			pcm2705HidCmd(PCM2705_HID_NEXT_TRACK);
			break;
		case CMD_RC5_PREV_TRACK:
		case CMD_BTN_4:
			pcm2705HidCmd(PCM2705_HID_PREV_TRACK);
			break;
		case CMD_RC5_STOP:
		case CMD_BTN_5:
			pcm2705HidCmd(PCM2705_HID_STOP);
			break;
		case CMD_RC5_PLAY_PAUSE:
		case CMD_BTN_6:
			pcm2705HidCmd(PCM2705_HID_PLAY_PAUSE);
			break;
		default:
			break;
		}

	}

	return 0;
}
