#ifndef PCM2705_H
#define PCM2705_H

#include <inttypes.h>

/* Hid commands definitions */
#define PCM2705_HID_NO_CMD			0x00
#define PCM2705_HID_MUTE			0x01
#define PCM2705_HID_VOL_UP			0x02
#define PCM2705_HID_VOL_DOWN		0x04
#define PCM2705_HID_NEXT_TRACK		0x08
#define PCM2705_HID_PREV_TRACK		0x10
#define PCM2705_HID_STOP			0x20
#define PCM2705_HID_PLAY_PAUSE		0x40
#define PCM2705_HID_EXTENDED		0x80

void pcm2705Init(void);
void pcm2705HidCmd(uint8_t cmd);

#endif /* PCM2705_H */
