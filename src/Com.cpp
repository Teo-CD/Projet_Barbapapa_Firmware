//
// Created by Teo-CD on 24/09/23.
//

#include "Com.h"

namespace Com {
	byte buffer[64];
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
		memset(buffer, 0, 64);
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
}
