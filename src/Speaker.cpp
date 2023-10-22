//
// Created by Teo-CD on 22/10/23.
//

#include "Speaker.h"

DMAMEM audio_block_t Speaker::audioMemory[audioMemReservation];
bool Speaker::memoryInitialized = false;

void Speaker::init() {
	if (memoryInitialized)
		return;
	AudioStream::initialize_memory(audioMemory, audioMemReservation);
	memoryInitialized = true;
}

void Speaker::playNewSound(int8_t ID) {
	char filePath[20];
	uint8_t rawID = getRawId(ID);

	if (!SD.mediaPresent()) {
		Com::sendComment("SD Card not present, cannot play sound");
		return;
	}
	if (!isValidId(ID)) {
		Com::sendComment("Unknown ID for sound : %d", rawID);
		return;
	}

	if (sdWavSource.isPlaying())
		sdWavSource.stop();

	snprintf(filePath, 20, "%s/ENTRY.WAV",
			 (isIdCharacter(ID) ? charDirs : promoDirs)[rawID]);

	if (!sd.exists(filePath)) {
		Com::sendComment("File %s not present, cannot play sound.", filePath);
		return;
	}
	digitalWrite(pin_Audio_Amp_enable, HIGH);
	sdWavSource.play(filePath);
}

bool Speaker::checkPlayingAndDisable() {
	bool isPlaying = sdWavSource.isPlaying();
	if (!isPlaying)
		digitalWrite(pin_Audio_Amp_enable, LOW);
	return isPlaying;
}
