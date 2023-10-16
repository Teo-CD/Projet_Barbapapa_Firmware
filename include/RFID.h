//
// Created by trotfunky on 10/09/23.
//

#ifndef JIN_BARBAPAPA_RFID_H
#define JIN_BARBAPAPA_RFID_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Metro.h>

#include <MFRC522v2.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522DriverSPI.h>

#include "Pinout.h"

/***
 * Linked list containing the uids of the tags and the corresponding IDs.
 */
struct uidNode {
	MFRC522Constants::Uid uid{};
	uidNode* next = nullptr;
	// IDs should be positive integers < 127 as the sign bit is used for direction.
	int8_t tag_ID = 0;
};

class RFID {
public:
	RFID() : comData{}, sd(SD) {};
	void init();
	/***
	 * Check if any new tags are present or have left. Only one event is reported
	 * at a time, another call would be needed to get eventual concurrent events.
	 * This updates the respective lists and returns the ID with the direction
	 * of the change.
	 * @return 0 if no change, ID if new tag detected, ID & sign bit if the tag left.
	 */
	int8_t checkTags();
private:
	/* Used for "encrypted" communication with the tags. */
	static MFRC522Constants::MIFARE_Key defaultKey;
	static constexpr int maxTags = 2;
	int currentActiveTags = 0;

	/*
	 * The linked list tracking active tags.
	 * It is statically allocated so that the memory footprint is constant.
	 */
	uidNode activeTagsArray[maxTags];
	uidNode* activeTags = nullptr;
	uidNode* nextFreeTagSlot = activeTagsArray;

	MFRC522DriverPinSimple driverPin{pin_NFC1_CS};
	MFRC522DriverSPI driverSpi{driverPin};
	MFRC522 mfrc{driverSpi};

	/*
	 * Array used to read and write blocks to the tags. Blocks are 16 bytes but
	 * during reads a two-bytes CRC is added.
	 */
	byte comData[18];

	SDClass &sd;
	Metro checkGoneTimer{250};

	/***
	 * Add a tag to the static linked list.
	 * @return The new node, or nullptr if all nodes are allocated.
	 */
	uidNode* addActiveTag(const MFRC522Constants::Uid &newTagUid);
	/***
	 * Remove goneNode from the list and optionally update its predecessor.
	 * @param goneNode Node to remove from the list.
	 * @param previousNode Node to update, can be nullptr if goneNode is the head.
	 */
	void removeActiveTag(uidNode* goneNode, uidNode* previousNode);

	/**
	 * Read block at blockADdr from tagUid and return if the read succeeded or not.
	 * The read block will be in comData.
	 * @param tagUid Uid of the tag to be read.
	 * @param blockAddr Address of the block to be read.
	 * @return True if read succeeded, false otherwise.
	 */
	bool readBlock(MFRC522Constants::Uid &tagUid, byte blockAddr);
};

#endif //JIN_BARBAPAPA_RFID_H
