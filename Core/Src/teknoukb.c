#include "teknoukb.h"

extern UART_HandleTypeDef huart2;
extern bno055_vector_t ivme;
extern bno055_vector_t euler;
extern float irtifaFiltre;
extern float basincFiltre;

TestModlari aktifMod = MOD_UCUS;

uint16_t durum;
uint8_t telePaket[36];
uint8_t fazPaket[6];

uint8_t header = 0xAA;
uint8_t footer1 = 0x0D;
uint8_t footer2 = 0x0A;

void modGuncelle(uint8_t komut){
	if(komut == 0x20) aktifMod = MOD_SIT;
	else if(komut == 0x22) aktifMod = MOD_SUT;
	else if(komut == 0x24) aktifMod = MOD_UCUS;
}

void teleGonder(){
	FloatDonusturucu cevirici;
	uint32_t teleChecksumToplam;
	uint8_t teleChecksum;
	uint8_t index = 0;

	telePaket[index++] = header;

	cevirici.giren = irtifaFiltre;
	telePaket[index++] = cevirici.cikan[0];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[3];

	cevirici.giren = basincFiltre;
	telePaket[index++] = cevirici.cikan[0];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[3];

	cevirici.giren = ivme.x;
	telePaket[index++] = cevirici.cikan[0];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[3];

	cevirici.giren = ivme.y;
	telePaket[index++] = cevirici.cikan[0];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[3];

	cevirici.giren = ivme.z;
	telePaket[index++] = cevirici.cikan[0];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[3];

	cevirici.giren = euler.x;
	telePaket[index++] = cevirici.cikan[0];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[3];

	cevirici.giren = euler.y;
	telePaket[index++] = cevirici.cikan[0];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[3];

	cevirici.giren = euler.z;
	telePaket[index++] = cevirici.cikan[0];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[3];

	for(int i=0;i<index;i++){
		teleChecksumToplam += telePaket[i];
	}

	teleChecksum = teleChecksumToplam % 256;
	telePaket[index++] = teleChecksum;

	telePaket[index++] = footer1;
	telePaket[index++] = footer2;

	HAL_UART_Transmit(&huart2, telePaket, sizeof(telePaket), 100);
}

void fazGonder(){
	uint32_t fazChecksumToplam;
	uint8_t fazChecksum;
	uint8_t index = 0;

	fazPaket[index++] = header;
	fazPaket[index++] = (uint8_t)(durum & 0xFF);
	fazPaket[index++] = (uint8_t)((durum >> 8) & 0xFF);

	for(int i=0;i<index;i++){
		fazChecksumToplam += fazPaket[i];
	}
	fazChecksum = fazChecksumToplam % 256;
	fazPaket[index++] = fazChecksum;

	fazPaket[index++] = footer1;
	fazPaket[index++] = footer2;

	HAL_UART_Transmit(&huart2, fazPaket, sizeof(fazPaket), 100);
}
