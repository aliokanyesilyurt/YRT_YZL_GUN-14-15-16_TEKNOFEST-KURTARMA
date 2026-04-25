#ifndef TEKNOUKB_H
#define TEKNOUKB_H

#include "main.h"
#include "newfunctions.h"

typedef enum {
	MOD_UCUS,
	MOD_SIT,
	MOD_SUT
} TestModlari;

typedef union {
	float giren;
	uint8_t cikan[4];
} FloatDonusturucu;

void modGuncelle();
void teleGonder();
void fazGonder();

extern TestModlari aktifTest;

#endif
