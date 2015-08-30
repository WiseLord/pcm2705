#include <util/delay.h>
#include <avr/interrupt.h>

#include "pins.h"
#include "pcm2705.h"
#include "input.h"
#include "rc5.h"

typedef enum {
	MODE_WORK,
	MODE_LEARN,

	MODE_END
} Mode;

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
	CmdID cmd = CMD_END;
	Mode mode = MODE_WORK;

	hwInit();

	while(1) {
		cmd = getCommand();

		/* Flash once on any simple command */
		if (cmd != CMD_END && (mode == MODE_LEARN || cmd != CMD_RC5_END))
			ledFlash(1);

		if (cmd == CMD_LEARN_MODE) {
			if (mode == MODE_WORK) {
				mode = MODE_LEARN;
			} else {
				mode = MODE_WORK;
			}
			ledFlash(3);
		} else if (cmd >= CMD_BTN_MUTE && cmd <= CMD_BTN_PLAY) {
			if (mode == MODE_WORK) {
				pcm2705HidCmd(1<<(cmd - CMD_BTN_MUTE));
			} else if (mode == MODE_LEARN) {
				rc5SaveButton(cmd - CMD_BTN_MUTE);
				ledFlash(2);
			}
		} else if (cmd >= CMD_RC5_MUTE && cmd <= CMD_RC5_PLAY) {
			if (mode == MODE_WORK) {
				pcm2705HidCmd(1<<(cmd - CMD_RC5_MUTE));
			}
		}
	}

	return 0;
}
