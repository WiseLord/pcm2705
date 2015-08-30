#ifndef PINS_H
#define PINS_H

#include <avr/io.h>
#include <avr/iom8.h>

#define CONCAT(x,y)			x ## y

#define DDR(x)				CONCAT(DDR,x)
#define PORT(x)				CONCAT(PORT,x)
#define PIN(x)				CONCAT(PIN,x)

#define LED					D
#define LED_LINE			(1<<7)

#define PCM2705_DPLUS		B
#define PCM2705_DPLUS_LINE	(1<<1)

#define PCM2705_MD			B
#define PCM2705_MD_LINE		(1<<7)
#define PCM2705_MC			D
#define PCM2705_MC_LINE		(1<<5)
#define PCM2705_MS			B
#define PCM2705_MS_LINE		(1<<6)

#define RC5					D
#define RC5_LINE			(1<<3)

/* Buttons definitions */
#define BTN_MUTE			B
#define BTN_MUTE_LINE		(1<<0)
#define BTN_VOLUP			C
#define BTN_VOLUP_LINE		(1<<3)
#define BTN_VOLDN			B
#define BTN_VOLDN_LINE		(1<<4)
#define BTN_NEXT			B
#define BTN_NEXT_LINE		(1<<5)
#define BTN_PREV			B
#define BTN_PREV_LINE		(1<<3)
#define BTN_STOP			C
#define BTN_STOP_LINE		(1<<1)
#define BTN_PLAY			C
#define BTN_PLAY_LINE		(1<<2)

#endif /* PINS_H */
