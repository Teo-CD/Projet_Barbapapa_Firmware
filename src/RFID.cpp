//
// Created by Teo-CD on 10/09/23.
//

#include "RFID.h"

static_assert(RFID::maxTags * sizeof(Com::TagInfoRecord) <= 62,
			  "RFID::maxTags too high, protocol cannot support it.\n");

MFRC522Constants::MIFARE_Key RFID::defaultKey = {0xFF,0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
int RFID::currentActiveTags = 0;

void RFID::init() {
	// Create and set up a static linked list to manage active tags.
	for (int i = 0; i < maxTags - 1; i++) {
		activeTagsArray[i].next = &activeTagsArray[i+1];
	}

	mfrc.PCD_Init();
}

int8_t RFID::checkTags() {
	// First check if some tags are gone. Check for new tags later.
	if (currentActiveTags && checkGoneTimer.check()) {
		uidNode* previousNode = nullptr;
		uidNode* node = activeTags;

		while (node) {
			byte comSize = sizeof(comData);

			mfrc.uid = node->uid;
			mfrc.PICC_WakeupA(comData, &comSize);

			if (mfrc.PICC_Select(&node->uid, node->uid.size) == MFRC522Constants::STATUS_OK) {
				// If we get a response : the tag is still here, halt it again and continue.
				mfrc.PICC_HaltA();
				previousNode = node;
				node = node->next;
//				Serial.printf("UID 0x%lX is still present\n", *(uint32_t *)previousNode->uid.uidByte);
			} else {
//				Serial.printf("UID 0x%lX is gone\n", *(uint32_t *)node->uid.uidByte);
				// If we don't, assume the tag is gone and remove it from the list.
				removeActiveTag(node, previousNode);
				// Return the ID with the sign bit set to signal it has gone.
				// If other tags are gone we can check on the next loop.
				return node->tag_ID | idMaskGone;
			}
		}
	}

	// If we still have room, check for new tags and read their serials.
	if ( currentActiveTags >= maxTags ||
		 !mfrc.PICC_IsNewCardPresent() ||
		 !mfrc.PICC_ReadCardSerial()) {
		return 0;
	}

	// Assume we have a new tag.
	uidNode* newTag = addActiveTag(mfrc.uid);
	// Tag ID is put in a specific block, which can be read after from comData.
	if ( !newTag ) {
		return 0;
	}
	// If the read fails, we cannot use this tag anyway so remove it from the list.
	if (!readBlock(newTag->uid, tagIdBlock) ) {
		removeActiveTag(newTag, nullptr);
		return 0;
	}
	// Don' t check the ID, otherwise we cannot program the tag.
	newTag->tag_ID = comData[0];
	return newTag->tag_ID;
}

uint8_t RFID::gatherTagInfo(Com::TagInfoRecord *tagData) {
	int tagCount = 0;
	for (uidNode* node = activeTags; node; node = node->next) {
		// The protocol does not support UIDs other than 4 bytes, so assume it
		// is the case here directly.
		tagData[tagCount].uid = *(uint32_t*)(node->uid.uidByte);
		tagData[tagCount].figId = node->tag_ID;
		tagCount++;
	}
	return tagCount;
}

bool RFID::programTag(Com::TagInfoRecord *tagToProgram) {

	byte comSize = sizeof(comData);

	mfrc.uid.size = 4;
	*(uint32_t*)(mfrc.uid.uidByte) = tagToProgram->uid;
	mfrc.PICC_WakeupA(comData, &comSize);
	if (mfrc.PICC_Select(&mfrc.uid, mfrc.uid.size) != MFRC522Constants::STATUS_OK) {
		Serial.println("Failed to wakeup for programming. Is tag still there ?\n");
		mfrc.PICC_HaltA();
		return false;
	}

	if (!writeBlock(mfrc.uid, tagIdBlock, tagToProgram->figId)) {
		Serial.println("Failed to program tag.");
		return false;
	}
	return true;
}

uidNode* RFID::addActiveTag(const MFRC522Constants::Uid &newTag) {
	if (!nextFreeTagSlot) {
		return nullptr;
	}
	uidNode* temp;
	temp = nextFreeTagSlot->next;
	// Move the top of the free list to the in use list.
	nextFreeTagSlot->next = activeTags;
	activeTags = nextFreeTagSlot;
	activeTags->uid = newTag;
	// Set the top of the free list to the next element, which we saved above.
	nextFreeTagSlot = temp;

	currentActiveTags++;
	return activeTags;
}

void RFID::removeActiveTag(uidNode* goneNode, uidNode* previousNode) {
	// Handle the edge case where we remove the last active tag.
	if (previousNode) {
		previousNode->next = goneNode->next;
	} else {
		activeTags = goneNode->next;
	}
	goneNode->next = nextFreeTagSlot;
	nextFreeTagSlot = goneNode;

	currentActiveTags--;
}

bool RFID::readBlock(MFRC522Constants::Uid &uidToRead, byte blockAddr) {
	if (mfrc.PCD_Authenticate(
			MFRC522Constants::PICC_Command::PICC_CMD_MF_AUTH_KEY_A,
			blockAddr, (&defaultKey),&uidToRead)!= MFRC522Constants::STATUS_OK) {
		Serial.println("readBlock: Failed to authenticate");
		mfrc.PICC_HaltA();
		return false;
	}

	byte size = sizeof(comData);
	MFRC522::StatusCode status = mfrc.MIFARE_Read(blockAddr, comData, &size);
//	Serial.printf("Read block : 0x%lX\n", *(uint32_t *)comData);
	// Needed otherwise no new communications can happen.
	mfrc.PCD_StopCrypto1();
	// No need to keep the tag active.
	mfrc.PICC_HaltA();
	return status == MFRC522Constants::STATUS_OK;
}

bool RFID::writeBlock(MFRC522Constants::Uid &uidToRead, byte blockAddr, uint8_t data) {
	if (mfrc.PCD_Authenticate(
			MFRC522Constants::PICC_Command::PICC_CMD_MF_AUTH_KEY_A,
			blockAddr, (&defaultKey),&uidToRead)!= MFRC522Constants::STATUS_OK) {
		Serial.println("writeBlock: Failed to authenticate");
		mfrc.PICC_HaltA();
		return false;
	}

	byte size = sizeof(comData);
	for (byte& bufferByte: comData)
		bufferByte = 0;

	*comData = data;

	MFRC522::StatusCode status = mfrc.MIFARE_Write(blockAddr, comData, size);
	// Needed otherwise no new communications can happen.
	mfrc.PCD_StopCrypto1();
	// No need to keep the tag active.
	mfrc.PICC_HaltA();
	return status == MFRC522Constants::STATUS_OK;
}
