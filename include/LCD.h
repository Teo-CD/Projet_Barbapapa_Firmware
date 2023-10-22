//
// Created by Teo-CD on 17/10/23.
//

#ifndef JIN_BARBAPAPA_LCD_H
#define JIN_BARBAPAPA_LCD_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#include <Adafruit_PCD8544.h>

#include "Audio.h"
#include "Com.h"
#include "IDs.h"
#include "Pinout.h"

class LCD {
public:
	LCD() : sd(SD) {};
	void init();
	/***
	 * Check if time has passed and we should display a new frame.
	 * @return True if a new frame was displayed.
	 */
	bool checkAndDisplay();
	void startNewAnim(int8_t ID);
private:
	Adafruit_PCD8544 lcd{pin_LCD_DC, pin_LCD_CS, pin_LCD_RST};

	SDClass &sd;
	/* Directory where the current animation frames are stored. */
	File animDir;

	/***
	 * Array storing the bitmap that will be passed to the LCD.
	 * The LCD is 84x48 so that should be 504 bytes, but the row length (84)
	 * is not byte aligned. Thus, the last byte is padded, giving 88x48 = 528.
	 */
	static uint8_t bitmap[528];
	uint32_t lastFrameTime = 0;
	/* Delay between two frames, in milliseconds. Above 10 FPS is mushy, avoid. */
	static constexpr uint32_t frameTarget = 1000 / 7;
};

#endif //JIN_BARBAPAPA_LCD_H
