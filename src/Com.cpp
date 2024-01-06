//
// Created by Teo-CD on 24/09/23.
//

#include "Com.h"
#include "RFID.h"

namespace Com {
	byte buffer[64];
	TagInfoRecord records[RFID::maxTags] = {};

	/**
 	 * Will flush buffer through either HID or Serial depending on build
 	 * options.
 	 * HID is the expected production usage as the interactive software will
 	 * use it for auto-detection.
 	 */
	inline void flushBuffer() {
#ifdef USB_RAWHID
		usb_rawhid_send(buffer, 0);
#else
		Serial.write((const char*)buffer);
#endif
		/* Clear the buffer. */
		memset(buffer, 0, sizeof(buffer));
	}

	void sendFigUpdate(int8_t event) {
		buffer[0] = '@';
		buffer[1] = event;
		flushBuffer();
	}

	void sendComment(const char message[62], ...) {
		va_list args;
		va_start(args, message);
		buffer[0] = '#';
		buffer[1] = vsnprintf((char *)(buffer + 2), 62, message, args);
		va_end(args);
		flushBuffer();
	}

	void sendTagInfo(uint8_t tagCount) {
		buffer[0] = '!';
		buffer[1] = tagCount;
		memcpy(buffer + 2, records, sizeof(TagInfoRecord)*tagCount);
		flushBuffer();
	}

	TagInfoRecord* getTagRecords() {
		return records;
	}

	inline bool dataAvailable() {
#ifdef USB_RAWHID
		return usb_rawhid_available();
#else
		/* Serial is used for debug, do not care about message length. */
		return Serial.available();
#endif
	}

	inline void receiveData() {
		memset(buffer, 0, sizeof(buffer));
#ifdef USB_RAWHID
		/* We know we have data, don't need to set a timeout. */
		usb_rawhid_recv(buffer, 0);
#else
		/* Could lead to incomplete messages, but should be OK on Teensy. */
		Serial.readBytes(buffer, Serial.available());
#endif
	}

	ReceivedMessage receiveMessage() {
		if (!dataAvailable())
			return NO_MESSAGE;
		receiveData();

		ReceivedMessage messageType;
		switch (buffer[0]) {
			case '?':
				messageType = TAG_INFO_REQUEST;
				break;
			case '=':
				messageType = TAG_PROGRAMMING;
				/* Make the record available to the rest of the code. */
				memset(records, 0, sizeof(records));
				memcpy(records, buffer + 1, sizeof(TagInfoRecord));
				break;
			default:
				messageType = INVALID_MESSAGE;
				Serial.printf("Invalid header received : %c\n", buffer[0]);
		}
		return messageType;
	}
}
