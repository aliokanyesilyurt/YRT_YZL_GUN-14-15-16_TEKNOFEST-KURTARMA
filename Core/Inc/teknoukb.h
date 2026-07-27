#ifndef TEKNOUKB_H
#define TEKNOUKB_H

#include "main.h"
#include "newfunctions.h"
#include "math.h"
typedef enum {
	MOD_UCUS,
	MOD_SIT,
	MOD_SUT
} TestModlari;

typedef union {
	float giren;
	uint8_t cikan[4];
} FloatDonusturucu;

void modGuncelle(uint8_t komut);
void teleGonder(void);
void fazGonder(void);
void sahteAl(uint8_t *sahteTele);
float yuzdeYuvarla(float deger);
extern TestModlari aktifMod;

#endif
