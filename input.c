#include "input.h"

#include "pins.h"
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "remote.h"
#include "eeprom.h"

static volatile CmdID cmdBuf;

/* Previous state */
static volatile uint16_t rc5SaveBuf;
static volatile uint8_t btnSaveBuf;
static volatile uint8_t btnPrev = BTN_STATE_IDLE;

static uint8_t rcType;
static uint8_t rcAddr;
static uint8_t rcCode[CMD_RC_END];					/* Array with rc5 commands */

static uint8_t ledTimer;
static uint8_t learn;

static CmdID rcCmdIndex(uint8_t rcCmd)
{
	CmdID i;

	for (i = 0; i < CMD_RC_END; i++)
		if (rcCmd == rcCode[i])
			return i;

	return CMD_RC_END;
}

void rcCodesInit(void)
{
	rcType = eeprom_read_byte((uint8_t*)EEPROM_RC_TYPE);
	rcAddr = eeprom_read_byte((uint8_t*)EEPROM_RC_ADDR);
	eeprom_read_block(rcCode, (uint8_t*)EEPROM_RC_CMD, CMD_RC_END);

	return;
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

	TCCR0 |= (0<<CS02) | (1<<CS01) | (1<<CS00);		/* Prescaler = 64, 125kHz */
	TIMSK |= (1<<TOIE0);							/* Enable timer compare match interrupt */

	rcCodesInit();

	cmdBuf = CMD_END;
	ledTimer = 0;

	return;
}

void rc5SaveButton(CmdID cmdID)
{
	IRData irBuf = getIrData();

	eeprom_update_byte((uint8_t*)EEPROM_RC_TYPE, irBuf.type);
	eeprom_update_byte((uint8_t*)EEPROM_RC_ADDR, irBuf.address);
	eeprom_update_byte((uint8_t*)EEPROM_RC_CMD + cmdID, irBuf.command);

	// Re-read new codes array from EEPROM
	rcCodesInit();

	return;
}

ISR (TIMER0_OVF_vect)
{
	static int16_t btnCnt = 0;						/* Buttons press duration value */
	static uint16_t rcTimer;

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
	IRData ir = takeIrData();

	CmdID rcCmdBuf = CMD_END;

	if (ir.ready && (learn ? 1 : (ir.type == rcType && ir.address == rcAddr))) {
		if (!ir.repeat || (rcTimer > 800)) {
			rcTimer = 0;
			rcCmdBuf = rcCmdIndex(ir.command);
		}
		if (ir.command == rcCode[CMD_RC_VOL_UP] || ir.command == rcCode[CMD_RC_VOL_DOWN]) {
			if (rcTimer > 400) {
				rcTimer = 360;
				rcCmdBuf = rcCmdIndex(ir.command);
			}
		}
	}

	if (cmdBuf == CMD_END)
		cmdBuf = rcCmdBuf;

	/* Time from last IR command */
	if (rcTimer < 1000)
		rcTimer++;

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

void setLearn(uint8_t status)
{
	learn = status;

	return;
}
