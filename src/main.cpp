#include <Arduino.h>

#include "Pinout.h"

#include "Com.h"
#include "LCD.h"
#include "RFID.h"
#include "Speaker.h"

__attribute__((noreturn)) int main() {
	pinMode(pin_DBG_LED_1, OUTPUT);
	pinMode(pin_DBG_LED_2, OUTPUT);

	pinMode(pin_Reg_en, OUTPUT);
	pinMode(pin_Audio_Amp_enable, OUTPUT);
	pinMode(pin_Audio_AMP_GAIN_EN, OUTPUT);

	pinMode(pin_NFC1_RST, OUTPUT);
	pinMode(pin_LCD_RST, OUTPUT);

	/* Preemptively disable what we can before the regulator is ON. */
	digitalWrite(pin_Audio_Amp_enable, LOW);
	digitalWrite(pin_NFC1_RST, LOW);
	digitalWrite(pin_LCD_RST, LOW);

	pinMode(pin_BAT_SNS, INPUT);
	/*
	 * Voltage divider ratio is 1.5 and analog input has 10 significant bits.
	 * Which gives us 0-1023 <=> 0V-4.95V, so 650 is a measure of 3.14V.
	 * This is quite low, but there's a voltage drop because of the diode, and
	 * we don't want to be at the very end of the battery's charge anyway.
	 * Let's be conservative.
	 */
	if (analogRead(pin_BAT_SNS) < 650) { /* TODO: Low battery handling */ }

	digitalWrite(pin_Reg_en, HIGH);
	/*
	 * System is powered up after this point.
	 */

	Serial.begin(115200);
	Com::sendComment("System is powered up, running set-up.");

	RFID rfid;
	digitalWrite(pin_NFC1_RST, HIGH);
	rfid.init();

	if (!SD.begin(BUILTIN_SDCARD))
		Com::sendComment("Could not use the SD Card.");

	LCD lcd;
	digitalWrite(pin_LCD_RST, HIGH);
	lcd.init();

	/* Don't enable the amp here, only do it when playing to save some power. */
	Speaker::init();
	Speaker speaker;

	Com::sendComment("All modules initialized, entering main loop.");

	/* Main loop */
	while (true) {
		int8_t tagEvent;

		/* Display first, in case the RFID communication delays it too much. */
		lcd.checkAndDisplay();
		speaker.checkPlayingAndDisable();

		tagEvent = rfid.checkTags();
		if (tagEvent) {
			tagEvent = 4;
			Com::sendFigUpdate(tagEvent);
			/* Start the audio first because of the possible WAV parsing delay. */
			speaker.playNewSound(tagEvent);
			lcd.startNewAnim(tagEvent);

		}

		/* TODO: Drop delay, WFE+timer interrupt(s) ? */
		delay(25);
	}
}
