#include <Arduino.h>

#include "Pinout.h"

#include "RFID.h"

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
	Serial.println("# System is powered up, running set-up.");

	/* TODO: Setups once module structure is up. */
	RFID rfid;
	digitalWrite(pin_NFC1_RST, HIGH);
	rfid.init();

	/* Main loop */
	while (true) {
		int8_t tagID;

		tagID = rfid.checkTags();
		if (tagID) {
			Serial.printf("Check tag result : %d\n", tagID);
		}

		delay(100);
	}
}
