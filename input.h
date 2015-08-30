#ifndef INPUT_H
#define INPUT_H

#include <inttypes.h>

#define BTN_STATE_IDLE			0x00
#define BTN_STATE_MUTE			0x01
#define BTN_STATE_VOLUP			0x02
#define BTN_STATE_VOLDN			0x04
#define BTN_STATE_NEXT			0x08
#define BTN_STATE_PREV			0x10
#define BTN_STATE_STOP			0x20
#define BTN_STATE_PLAY			0x40
#define BTN_STATE_EXT			0x80

typedef enum {
	CMD_RC5_MUTE,
	CMD_RC5_VOLUP,
	CMD_RC5_VOLDN,
	CMD_RC5_NEXT,
	CMD_RC5_PREV,
	CMD_RC5_STOP,
	CMD_RC5_PLAY,

	CMD_RC5_END,

	CMD_BTN_MUTE,
	CMD_BTN_VOLUP,
	CMD_BTN_VOLDN,
	CMD_BTN_NEXT,
	CMD_BTN_PREV,
	CMD_BTN_STOP,
	CMD_BTN_PLAY,

	CMD_BTN_MUTE_LONG,
	CMD_BTN_VOLUP_LONG,
	CMD_BTN_VOLDN_LONG,
	CMD_BTN_NEXT_LONG,
	CMD_BTN_PREV_LONG,
	CMD_BTN_STOP_LONG,
	CMD_BTN_PLAY_LONG,

	CMD_END

} cmdID;

/* Handling long press actions */
#define SHORT_PRESS				50
#define LONG_PRESS				300
#define AUTOREPEAT				100

void inputInit();

cmdID getBtnCmd(void);

uint16_t getRC5Buf(void);
uint16_t getBtnBuf(void);

void ledFlash(uint16_t time);

#endif /* INPUT_H */
