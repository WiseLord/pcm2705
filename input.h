#ifndef INPUT_H
#define INPUT_H

#include <inttypes.h>

#define BTN_STATE_0				0x00
#define BTN_0					0x01
#define BTN_1					0x02
#define BTN_2					0x04
#define BTN_3					0x08
#define BTN_4					0x10
#define BTN_5					0x20
#define BTN_6					0x40

/* RC5 support definitions */
#define RC5_CMD_COUNT			7

typedef enum {
	CMD_RC5_MUTE,
	CMD_RC5_VOL_UP,
	CMD_RC5_VOL_DOWN,
	CMD_RC5_NEXT_TRACK,
	CMD_RC5_PREV_TRACK,
	CMD_RC5_STOP,
	CMD_RC5_PLAY_PAUSE,

	CMD_BTN_0,
	CMD_BTN_1,
	CMD_BTN_2,
	CMD_BTN_3,
	CMD_BTN_4,
	CMD_BTN_5,
	CMD_BTN_6,
	CMD_BTN_0_LONG,
	CMD_BTN_1_LONG,
	CMD_BTN_2_LONG,
	CMD_BTN_3_LONG,
	CMD_BTN_4_LONG,
	CMD_BTN_5_LONG,
	CMD_BTN_6_LONG,
	CMD_BTN_0_REPEAT,
	CMD_BTN_1_REPEAT,
	CMD_BTN_2_REPEAT,
	CMD_BTN_3_REPEAT,
	CMD_BTN_4_REPEAT,
	CMD_BTN_5_REPEAT,
	CMD_BTN_6_REPEAT,

	CMD_EMPTY = 0xEF
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
