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
	void sendFigUpdate(int8_t event);
	void sendComment(const char message[62], ...);
}

#endif //JIN_BARBAPAPA_COM_H
