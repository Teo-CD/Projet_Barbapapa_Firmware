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

#include "Com.h"
#include "IDs.h"
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
	static constexpr int maxTags = 2;

	/***
	 * Use the main SPI port and NFC chip select by default, but allow choosing
	 * an alternate chip select and/or SPI bus if needed to allow multiple
	 * RFID readers in parallel.
	 */
	explicit RFID(const uint8_t chipSelect = pin_NFC1_CS, SPIClass &spiBus = SPI) :
		driverPin(chipSelect), driverSpi(driverPin, spiBus),
		mfrc(driverSpi), comData{}, sd(SD) {};

	void init();
	/***
	 * Check if any new tags are present or have left. Only one event is reported
	 * at a time, another call would be needed to get eventual concurrent events.
	 * This updates the respective lists and returns the ID with the direction
	 * of the change.
	 * @return 0 if no change, ID if new tag detected, ID & sign bit if the tag left.
	 */
	int8_t checkTags();

	/***
	 * Populate an already allocated array of sufficient size to fit
	 * maxTags number of TagInfoRecords.
	 * @param tagData Pointer to an array of size maxTags*sizeof(Com::TagInfoRecord)
	 * @return Number of tags actually put in the array.
	 */
	uint8_t gatherTagInfo(Com::TagInfoRecord* tagData);

	/***
	 * Change the ID stored in a tag so that it matches tagToProgram.
	 * @param tagToProgram Pointer to an existing struct with the data to program.
	 * @return True if the programming succeeded.
	 */
	bool programTag(Com::TagInfoRecord* tagToProgram);
private:
	static const byte tagIdBlock = 0x11;
	/* Used for "encrypted" communication with the tags. */
	static MFRC522Constants::MIFARE_Key defaultKey;
	static int currentActiveTags;

	/*
	 * The linked list tracking active tags.
	 * It is statically allocated so that the memory footprint is constant.
	 */
	uidNode activeTagsArray[maxTags];
	uidNode* activeTags = nullptr;
	uidNode* nextFreeTagSlot = activeTagsArray;

	MFRC522DriverPinSimple driverPin;
	MFRC522DriverSPI driverSpi;
	MFRC522 mfrc;

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
	 * The tag needs to be already woken up.
	 * @param tagUid Uid of the tag to be read.
	 * @param blockAddr Address of the block to be read.
	 * @return True if read succeeded, false otherwise.
	 */
	bool readBlock(MFRC522Constants::Uid &tagUid, byte blockAddr);

	/**
	 * Write data to tagUid at blockAddr and return if the write succeeded or not.
	 * The tag needs to be already woken up.
	 * @param tagUid Uid of the tag to write to.
	 * @param blockAddr Address of the block to write to.
	 * @param data Byte to write.
	 * @return True if write succeeded, false otherwise.
	 */
	bool writeBlock(MFRC522Constants::Uid &tagUid, byte blockAddr, uint8_t data);
};

#endif //JIN_BARBAPAPA_RFID_H
