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
#define BUTTON_0			B
#define BUTTON_0_LINE		(1<<0)
#define BUTTON_1			C
#define BUTTON_1_LINE		(1<<3)
#define BUTTON_2			B
#define BUTTON_2_LINE		(1<<4)
#define BUTTON_3			B
#define BUTTON_3_LINE		(1<<5)
#define BUTTON_4			B
#define BUTTON_4_LINE		(1<<3)
#define BUTTON_5			C
#define BUTTON_5_LINE		(1<<1)
#define BUTTON_6			C
#define BUTTON_6_LINE		(1<<2)

#endif /* PINS_H */
