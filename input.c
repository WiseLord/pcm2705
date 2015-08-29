#include "input.h"

#include "pins.h"
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "rc5.h"
#include "eeprom.h"

static volatile cmdID cmdBuf;

/* Previous state */
static volatile uint16_t rc5SaveBuf;
static volatile uint8_t btnSaveBuf;
static volatile uint8_t btnPrev = BTN_STATE_0;

static uint8_t rc5DeviceAddr;
static uint8_t rcCode[CMD_BTN_0];				/* Array with rc5 commands */

uint16_t ledTimer;

void inputInit()
{
	uint8_t i;

	/* Setup LED */
	DDR(LED) |= LED_LINE;

	/* Setup buttons and encoder as inputs with pull-up resistors */
	DDR(BUTTON_0) &= ~BUTTON_0_LINE;
	DDR(BUTTON_1) &= ~BUTTON_1_LINE;
	DDR(BUTTON_2) &= ~BUTTON_2_LINE;
	DDR(BUTTON_3) &= ~BUTTON_3_LINE;
	DDR(BUTTON_4) &= ~BUTTON_4_LINE;
	DDR(BUTTON_5) &= ~BUTTON_5_LINE;
	DDR(BUTTON_6) &= ~BUTTON_6_LINE;

	PORT(BUTTON_0) |= BUTTON_0_LINE;
	PORT(BUTTON_1) |= BUTTON_1_LINE;
	PORT(BUTTON_2) |= BUTTON_2_LINE;
	PORT(BUTTON_3) |= BUTTON_3_LINE;
	PORT(BUTTON_4) |= BUTTON_4_LINE;
	PORT(BUTTON_5) |= BUTTON_5_LINE;
	PORT(BUTTON_6) |= BUTTON_6_LINE;

//	/* Set timer prescaller to 128 (125 kHz) and reset on match*/
//	TCCR2 = ((1<<CS22) | (0<<CS21) | (1<<CS20) | (1<<WGM21));
//	OCR2 = 125;										/* 125000/125 => 1000 polls/sec */
//	TCNT2 = 0;										/* Reset timer value */
//	TIMSK |= (1<<OCIE2);							/* Enable timer compare match interrupt */

	/* Set timer prescaller to 64 (250 kHz) */
	TIMSK0 |= (1<<TOIE0);							/* Enable Timer0 overflow interrupt */
	TCCR0B |= (0<<CS02) | (1<<CS01) | (1<<CS00);	/* No prescaler, 16M/64 = 250000 Hz */

	/* Load RC5 device address and commands from eeprom */
	rc5DeviceAddr = eeprom_read_byte((uint8_t*)EEPROM_RC5_ADDR);
	for (i = 0; i < RC5_CMD_COUNT; i++)
		rcCode[i] = eeprom_read_byte((uint8_t*)EEPROM_RC5_CMD + i);

	cmdBuf = CMD_EMPTY;
	ledTimer = 0;

	return;
}

static uint8_t rc5CmdIndex(uint8_t rc5Cmd)
{
	uint8_t i;

	for (i = 0; i < RC5_CMD_COUNT; i++)
		if (rc5Cmd == rcCode[i])
			return i;

	return CMD_EMPTY;
}

//ISR (TIMER2_COMP_vect)
ISR (TIMER0_OVF_vect)
{
	static int16_t btnCnt = 0;						/* Buttons press duration value */
	static uint16_t rc5Timer;

	/* Current state */
	uint8_t btnNow = BTN_STATE_0;

	if (~PIN(BUTTON_0) & BUTTON_0_LINE)
		btnNow |= BTN_0;
	if (~PIN(BUTTON_1) & BUTTON_1_LINE)
		btnNow |= BTN_1;
	if (~PIN(BUTTON_2) & BUTTON_2_LINE)
		btnNow |= BTN_2;
	if (~PIN(BUTTON_3) & BUTTON_3_LINE)
		btnNow |= BTN_3;
	if (~PIN(BUTTON_4) & BUTTON_4_LINE)
		btnNow |= BTN_4;
	if (~PIN(BUTTON_5) & BUTTON_5_LINE)
		btnNow |= BTN_5;
	if (~PIN(BUTTON_6) & BUTTON_6_LINE)
		btnNow |= BTN_6;

	/* If button event has happened, place it to command buffer */
	if (btnNow) {
		if (btnNow == btnPrev) {
			btnCnt++;
			if (btnCnt == LONG_PRESS) {
//				switch (btnPrev) {
//				case BTN_0:
//					cmdBuf = CMD_BTN_0_LONG;
//					break;
//				case BTN_1:
//					cmdBuf = CMD_BTN_1_LONG;
//					break;
//				case BTN_2:
//					cmdBuf = CMD_BTN_2_LONG;
//					break;
//				case BTN_3:
//					cmdBuf = CMD_BTN_3_LONG;
//					break;
//				case BTN_4:
//					cmdBuf = CMD_BTN_4_LONG;
//					break;
//				case BTN_5:
//					cmdBuf = CMD_BTN_5_LONG;
//					break;
//				case BTN_6:
//					cmdBuf = CMD_BTN_6_LONG;
//					break;
//				}
			} else if(btnCnt == LONG_PRESS + AUTOREPEAT) {
				switch (btnPrev) {
				case BTN_1:
					cmdBuf = CMD_BTN_1_REPEAT;
					break;
				case BTN_2:
					cmdBuf = CMD_BTN_2_REPEAT;
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
			case BTN_0:
				cmdBuf = CMD_BTN_0;
				break;
			case BTN_1:
				cmdBuf = CMD_BTN_1;
				break;
			case BTN_2:
				cmdBuf = CMD_BTN_2;
				break;
			case BTN_3:
				cmdBuf = CMD_BTN_3;
				break;
			case BTN_4:
				cmdBuf = CMD_BTN_4;
				break;
			case BTN_5:
				cmdBuf = CMD_BTN_5;
				break;
			case BTN_6:
				cmdBuf = CMD_BTN_6;
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

	uint8_t rc5CmdBuf = CMD_EMPTY;
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
		if (rc5Cmd == rcCode[CMD_RC5_VOL_UP] || rc5Cmd == rcCode[CMD_RC5_VOL_DOWN]) {
			if (rc5Timer > 400) {
				rc5Timer = 360;
				rc5CmdBuf = rc5CmdIndex(rc5Cmd);
			}
		}
		togBitPrev = togBitNow;
	}

	if (cmdBuf == CMD_EMPTY)
		cmdBuf = rc5CmdBuf;

	/* Time from last IR command */
	if (rc5Timer < 1000)
		rc5Timer++;

	if (ledTimer > 0)
		ledTimer--;
	else
		PORT(LED) &= ~LED_LINE;

	return;
};

cmdID getBtnCmd(void)
{
	cmdID ret;

	ret = cmdBuf;
	cmdBuf = CMD_EMPTY;

	return ret;
}

uint16_t getRC5Buf(void)
{
	return rc5SaveBuf;
}

uint16_t getBtnBuf(void)
{
	return btnSaveBuf;
}

void ledFlash(uint16_t time)
{
	ledTimer = time;
	PORT(LED) |= LED_LINE;

	return;
}
