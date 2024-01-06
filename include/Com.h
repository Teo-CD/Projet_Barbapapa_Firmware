//
// Created by Teo-CD on 24/09/23.
//

#ifndef JIN_BARBAPAPA_COM_H
#define JIN_BARBAPAPA_COM_H

#include <Arduino.h>
#include <cstring>

/**
 * Handle communication with a connected device and provide convenience
 * functions for the fixed protocols.
 */
namespace Com {
	enum ReceivedMessage {
		NO_MESSAGE,
		TAG_INFO_REQUEST,
		TAG_PROGRAMMING,
		INVALID_MESSAGE,
	};

	/**
	 * Struct used for tag info responses and tag programming.
	 * Would be 8 bytes if not packed due to padding.
	 */
	struct __attribute__((packed)) TagInfoRecord {
		uint32_t uid;
		uint8_t figId;
	};

	void sendFigUpdate(int8_t event);
	void sendComment(const char message[62], ...);
	/**
	 * Send the response to the tag info request.
	 * The TagInfoRecord buffer needs to be properly filled through getTagRecords().
	 * @param tagCount The amount of tags that are present in the response.
	 */
	void sendTagInfo(uint8_t tagCount);

	/**
	 * @return Pointer to a buffer of TagInfoRecords
	 */
	TagInfoRecord* getTagRecords();

	/**
	 * Check for any incoming message and handle it if needed, returning the
	 * kind of message received if known, no message if there was nothing to read
	 * and invalid message otherwise.
	 * @return Kind of message received.
	 */
	ReceivedMessage receiveMessage();
}

#endif //JIN_BARBAPAPA_COM_H
