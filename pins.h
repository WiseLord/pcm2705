#ifndef PINS_H
#define PINS_H

#include <avr/io.h>
#include "arduino.h"

#define CONCAT(x,y)			x ## y

#define DDR(x)				CONCAT(DDR,x)
#define PORT(x)				CONCAT(PORT,x)
#define PIN(x)				CONCAT(PIN,x)

#define LED					ARDUINO_TXLED
#define LED_LINE			ARDUINO_TXLED_LINE

#define PCM2705_DPLUS		ARDUINO_D10
#define PCM2705_DPLUS_LINE	ARDUINO_D10_LINE

#define PCM2705_MD			ARDUINO_D11
#define PCM2705_MD_LINE		ARDUINO_D11_LINE
#define PCM2705_MC			ARDUINO_D12
#define PCM2705_MC_LINE		ARDUINO_D12_LINE
#define PCM2705_MS			ARDUINO_D13
#define PCM2705_MS_LINE		ARDUINO_D13_LINE

#endif /* PINS_H */
