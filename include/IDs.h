//
// Created by Teo-CD on 19/10/23.
//

#ifndef JIN_BARBAPAPA_IDS_H
#define JIN_BARBAPAPA_IDS_H

#include <Arduino.h>

/***
 * Convenience functions and variables to handle IDs and data that they link to.
 */

constexpr uint8_t idMaskGone = 0b10000000;
constexpr uint8_t idMaskChar = 0b01000000;

bool isIdGone(int8_t ID);
bool isIdCharacter(int8_t ID);
/***
 * Get the ID without the control bits : direction and character/promotion bit.
 * @param ID to check, with all bits
 * @return The ID with only index bits.
 */
uint8_t getRawId(int8_t ID);
/***
 * Checks if the ID is known to the firmware, either as a character or promotion.
 * @param ID to check, with all bits
 * @return True if valid as a character or promotion, false otherwise.
 */
bool isValidId(int8_t ID);

static constexpr uint8_t dirStrSize = 10;
extern const char promoDirs[][dirStrSize];
extern const char charDirs[][dirStrSize];


#endif //JIN_BARBAPAPA_IDS_H
