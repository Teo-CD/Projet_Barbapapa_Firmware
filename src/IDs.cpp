//
// Created by Teo-CD on 19/10/23.
//

#include "IDs.h"

/* IDs are greater than one, offset the array to make it easier to use. */
constexpr char promoDirs[][dirStrSize] = {
		"INVALIDE",
		"RANDONNEE",
		"RESTO",
		"AUTOBUS",
		"MAYO",
		"MADO",
		"COOP",
		"OURS",
		"BAGAR",
		"INCAPABLE",
		"MYSTERE"
};
constexpr char charDirs[][dirStrSize] = {
		"INVALIDE",
		"GUILLAUME",
		"MICHEL"
};

bool isIdGone(int8_t ID) {
	return ID & idMaskGone;
}

bool isIdCharacter(int8_t ID) {
	return ID & idMaskChar;
}

uint8_t getRawId(int8_t ID) {
	return ID & ~(idMaskGone | idMaskChar);
}

bool isValidId(int8_t ID) {
	uint8_t rawId = getRawId(ID);
	if (isIdCharacter(ID))
		return rawId < sizeof(charDirs) / dirStrSize;
	else
		return rawId < sizeof(promoDirs) / dirStrSize;
}
