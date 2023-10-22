//
// Created by Teo-CD on 22/10/23.
//

#ifndef JIN_BARBAPAPA_SPEAKER_H
#define JIN_BARBAPAPA_SPEAKER_H

#include <Arduino.h>
#include <Audio.h>
#include <SerialFlash.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include "Com.h"
#include "IDs.h"
#include "Pinout.h"

static constexpr uint8_t audioMemReservation = 10;

class Speaker {
public:
	Speaker() : sd(SD) {};
	static void init();

	/** Play the entry sound for a specific tag ID. */
	void playNewSound(int8_t ID);

	/** Check if a sound and playing an disable the amp if not anymore. */
	bool checkPlayingAndDisable();
private:
	SDClass &sd;

	/*
	 * Below are members needed to set up the audio pipeline.
	 * We need an input, output, and to connect them to one another.
	 * We also need some specially reserved memory to store the data to playback.
	 */
	static audio_block_t audioMemory[audioMemReservation];
	static bool memoryInitialized;
	AudioPlaySdWav sdWavSource;
	AudioOutputI2S i2sOut;
	AudioConnection connectLeftChannels{sdWavSource, 0, i2sOut, 0};
	AudioConnection connectRightChannels{sdWavSource, 1, i2sOut, 1};
};


#endif //JIN_BARBAPAPA_SPEAKER_H
