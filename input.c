#include "input.h"

#include "pins.h"
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "rc5.h"
#include "eeprom.h"

static volatile CmdID cmdBuf;

/* Previous state */
static volatile uint16_t rc5SaveBuf;
static volatile uint8_t btnSaveBuf;
static volatile uint8_t btnPrev = BTN_STATE_IDLE;

static uint8_t rc5DeviceAddr;
static uint8_t rcCode[CMD_BTN_MUTE];				/* Array with rc5 commands */

static uint8_t ledTimer;

static CmdID rc5CmdIndex(uint8_t rc5Cmd)
{
	CmdID i;

	for (i = 0; i < CMD_RC5_END; i++)
		if (rc5Cmd == rcCode[i])
			return i;

	return CMD_RC5_END;
}

void inputInit()
{
	/* Setup LED */
	DDR(LED) |= LED_LINE;

	/* Setup buttons and encoder as inputs with pull-up resistors */
	DDR(BTN_MUTE) &= ~BTN_MUTE_LINE;
	DDR(BTN_VOLUP) &= ~BTN_VOLUP_LINE;
	DDR(BTN_VOLDN) &= ~BTN_VOLDN_LINE;
	DDR(BTN_NEXT) &= ~BTN_NEXT_LINE;
	DDR(BTN_PREV) &= ~BTN_PREV_LINE;
	DDR(BTN_STOP) &= ~BTN_STOP_LINE;
	DDR(BTN_PLAY) &= ~BTN_PLAY_LINE;

	PORT(BTN_MUTE) |= BTN_MUTE_LINE;
	PORT(BTN_VOLUP) |= BTN_VOLUP_LINE;
	PORT(BTN_VOLDN) |= BTN_VOLDN_LINE;
	PORT(BTN_NEXT) |= BTN_NEXT_LINE;
	PORT(BTN_PREV) |= BTN_PREV_LINE;
	PORT(BTN_STOP) |= BTN_STOP_LINE;
	PORT(BTN_PLAY) |= BTN_PLAY_LINE;

	TCCR0 |= (0<<CS02) | (1<<CS01) | (0<<CS00);		/* Prescaler = 8, 1M/8 = 125kHz */
	TIMSK |= (1<<TOIE0);							/* Enable timer compare match interrupt */

	/* Load RC5 device address and commands from eeprom */
	rc5DeviceAddr = eeprom_read_byte((uint8_t*)EEPROM_RC5_ADDR);
	eeprom_read_block(rcCode, (uint8_t*)EEPROM_RC5_CMD, CMD_RC5_END);

	cmdBuf = CMD_END;
	ledTimer = 0;

	return;
}

void rc5SaveButton(CmdID cmdID)
{
	uint8_t addr, cmd;

	addr = (rc5SaveBuf & 0x07C0)>>6;
	cmd = rc5SaveBuf & 0x003F;

	eeprom_update_byte((uint8_t*)EEPROM_RC5_ADDR, addr);
	eeprom_update_byte((uint8_t*)(EEPROM_RC5_CMD + cmdID), cmd);

	rc5DeviceAddr = addr;
	rcCode[cmdID] = cmd;

	return;
}

ISR (TIMER0_OVF_vect)
{
	static int16_t btnCnt = 0;						/* Buttons press duration value */
	static uint16_t rc5Timer;

	/* Current state */
	uint8_t btnNow = BTN_STATE_IDLE;

	if (~PIN(BTN_MUTE) & BTN_MUTE_LINE)
		btnNow |= BTN_STATE_MUTE;
	if (~PIN(BTN_VOLUP) & BTN_VOLUP_LINE)
		btnNow |= BTN_STATE_VOLUP;
	if (~PIN(BTN_VOLDN) & BTN_VOLDN_LINE)
		btnNow |= BTN_STATE_VOLDN;
	if (~PIN(BTN_NEXT) & BTN_NEXT_LINE)
		btnNow |= BTN_STATE_NEXT;
	if (~PIN(BTN_PREV) & BTN_PREV_LINE)
		btnNow |= BTN_STATE_PREV;
	if (~PIN(BTN_STOP) & BTN_STOP_LINE)
		btnNow |= BTN_STATE_STOP;
	if (~PIN(BTN_PLAY) & BTN_PLAY_LINE)
		btnNow |= BTN_STATE_PLAY;

	/* If button event has happened, place it to command buffer */
	if (btnNow) {
		if (btnNow == btnPrev) {
			btnCnt++;
			if (btnCnt == LONG_PRESS) {
				switch (btnPrev) {
				case BTN_STATE_LEARN:
					cmdBuf = CMD_LEARN_MODE;
					break;
				}
			} else if(btnCnt == LONG_PRESS + AUTOREPEAT) {
				switch (btnPrev) {
				case BTN_STATE_VOLUP:
					cmdBuf = CMD_BTN_VOLUP;
					break;
				case BTN_STATE_VOLDN:
					cmdBuf = CMD_BTN_VOLDN;
					break;
				}
				btnCnt = LONG_PRESS + 1;
			}
		} else {
			btnPrev = btnNow;
		}
	} else {
		if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS)) {
			switch (btnPrev) {
			case BTN_STATE_MUTE:
				cmdBuf = CMD_BTN_MUTE;
				break;
			case BTN_STATE_VOLUP:
				cmdBuf = CMD_BTN_VOLUP;
				break;
			case BTN_STATE_VOLDN:
				cmdBuf = CMD_BTN_VOLDN;
				break;
			case BTN_STATE_NEXT:
				cmdBuf = CMD_BTN_NEXT;
				break;
			case BTN_STATE_PREV:
				cmdBuf = CMD_BTN_PREV;
				break;
			case BTN_STATE_STOP:
				cmdBuf = CMD_BTN_STOP;
				break;
			case BTN_STATE_PLAY:
				cmdBuf = CMD_BTN_PLAY;
				break;
			}
		}
		btnCnt = 0;
	}
	btnSaveBuf = btnNow;

	/* Place RC5 event to command buffer if enough RC5 timer ticks */
	uint16_t rc5Buf = getRC5RawBuf();
	if (rc5Buf != RC5_BUF_EMPTY)
		rc5SaveBuf = rc5Buf;

	static uint8_t togBitNow = 0;
	static uint8_t togBitPrev = 0;

	CmdID rc5CmdBuf = CMD_END;
	uint8_t rc5Cmd;

	if ((rc5Buf != RC5_BUF_EMPTY) && ((rc5Buf & RC5_ADDR_MASK) >> 6 == rc5DeviceAddr)) {
		if (rc5Buf & RC5_TOGB_MASK)
			togBitNow = 1;
		else
			togBitNow = 0;

		rc5Cmd = rc5Buf & RC5_COMM_MASK;
		if ((togBitNow != togBitPrev) || (rc5Timer > 800)) {
			rc5Timer = 0;
			rc5CmdBuf = rc5CmdIndex(rc5Cmd);
		}
		if (rc5Cmd == rcCode[CMD_RC5_VOLUP] || rc5Cmd == rcCode[CMD_RC5_VOLDN]) {
			if (rc5Timer > 400) {
				rc5Timer = 360;
				rc5CmdBuf = rc5CmdIndex(rc5Cmd);
			}
		}
		togBitPrev = togBitNow;
	}

	if (cmdBuf == CMD_END)
		cmdBuf = rc5CmdBuf;

	/* Time from last IR command */
	if (rc5Timer < 1000)
		rc5Timer++;

	if (ledTimer > 0) {
		ledTimer--;
		if (ledTimer & 0x20)
			PORT(LED) |= LED_LINE;
		else
			PORT(LED) &= ~LED_LINE;
	}

	return;
};

CmdID getCommand(void)
{
	CmdID ret;

	ret = cmdBuf;
	cmdBuf = CMD_END;

	return ret;
}

void ledFlash(uint8_t time)
{
	ledTimer = time << 6;

	return;
}
