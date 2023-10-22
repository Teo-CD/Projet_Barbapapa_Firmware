//
// Created by Teo-CD on 17/10/23.
//

#include "LCD.h"

uint8_t LCD::bitmap[528] = {};

void LCD::init() {
	lcd.begin(60, 4);
	lcd.clearDisplay();
	lastFrameTime = millis();
	/* Needed to keep the display clean at boot. */
	lcd.display();
	/* TODO: JIN splash welcome ? */
}

void LCD::startNewAnim(int8_t ID) {
	char directoryPath[15];
	uint8_t rawID = getRawId(ID);

	if (!SD.mediaPresent()) {
		Com::sendComment("SD Card not present, cannot play animation");
		return;
	}
	if (!isValidId(ID)) {
		Com::sendComment("Unknown ID for animation : %d", rawID);
		return;
	}

	/* Interrupt previous running animation if there's one. */
	if (animDir)
		animDir.close();

	/* TODO: If adding remove animation, change path + check isIdGone */
	snprintf(directoryPath, 15, "%s/ANIM",
			 (isIdCharacter(ID) ? charDirs : promoDirs)[rawID]);
	animDir = SD.open(directoryPath);
	if (!animDir) {
		Com::sendComment("Could not open directory %s for ID %d", directoryPath, rawID);
		return;
	}
	checkAndDisplay();
}

bool LCD::checkAndDisplay() {
	if (!animDir || millis() - lastFrameTime < frameTarget)
		return false;

	/*
	 * Audio playback is controlled by interrupts, so it can happen anytime.
	 * Prevent a potential conflict when reading a frame of animation from the
	 * SD card and being interrupted by the audio library, which will try to
	 * read audio data from the SD card at the same time.
	 */
	AudioNoInterrupts();
	File frame = animDir.openNextFile();
	if (!frame) {
		/* There are no frames anymore, the animation is complete. */
		animDir.close();
		AudioInterrupts();
		return false;
	}
	frame.read(bitmap, 528);
	frame.close();
	AudioInterrupts();

	lcd.clearDisplay();
	lcd.drawBitmap(0, 0, bitmap, 84, 48, 1);
	lcd.display();

	lastFrameTime = millis();
	return true;
}
