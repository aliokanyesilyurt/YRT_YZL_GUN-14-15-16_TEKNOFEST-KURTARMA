#include "teknoukb.h"

extern UART_HandleTypeDef huart2;
extern bno055_vector_t ivme;
extern bno055_vector_t euler;
extern float irtifaFiltre;
extern float basincFiltre;

TestModlari aktifMod = MOD_UCUS;

uint16_t durum;
uint8_t telePaket[36] = {0};
uint8_t fazPaket[6] = {0};
uint8_t sahtePaket[36] = {0};

uint8_t header = 0xAB;
uint8_t footer1 = 0x0D;
uint8_t footer2 = 0x0A;

void modGuncelle(uint8_t komut){
	if(komut == 0x20) aktifMod = MOD_SIT;
	else if(komut == 0x22) aktifMod = MOD_SUT;
	else if(komut == 0x24) aktifMod = MOD_UCUS;
}

void durumGuncelle(void) {
    durum = 0;
    durum |= (ucusDurumu & 0xFF);

    if (ucusDurumu >= FAZ_FIRLATMA) durum |= (1 << 0);
        if (ucusDurumu >= FAZ_TIRMANIS) durum |= (1 << 1);
        if (ucusDurumu >= FAZ_ARAYIS)   durum |= (1 << 4);

    if (HAL_GPIO_ReadPin(TEPE_PA9_GPIO_Port, TEPE_PA9_Pin) == GPIO_PIN_SET) {
        durum |= (1 << 8);
    }
    if (HAL_GPIO_ReadPin(YEDEK_PA7_GPIO_Port, YEDEK_PA7_Pin) == GPIO_PIN_SET) {
        durum |= (1 << 9);
    }
}

void teleGonder(void){
	FloatDonusturucu cevirici;
	uint32_t teleChecksumToplam = 0;
	uint8_t teleChecksum = 0;
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

void fazGonder(void){
    durumGuncelle();
    uint8_t index = 0;

    fazPaket[index++] = 0xAA;
    fazPaket[index++] = (uint8_t)(durum & 0xFF);
    fazPaket[index++] = (uint8_t)((durum >> 8) & 0xFF);

    uint32_t fazChecksumToplam = fazPaket[0] + fazPaket[1] + fazPaket[2];
    fazPaket[index++] = fazChecksumToplam % 256;

    fazPaket[index++] = footer1; // 0x0D
    fazPaket[index++] = footer2; // 0x0A

    HAL_UART_Transmit(&huart2, fazPaket, 6, 100);
}

void sahteAl(uint8_t *sahteVeri){
    uint8_t temp[4];
    float tempFloat; // BU HAYAT KURTARACAK!
    uint8_t index = 1; // 0xAB'yi atla

    // KİLİDİ AL
    osMutexAcquire(SensorMutexHandle, osWaitForever);

    // İRTİFA
    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    irtifaFiltre = tempFloat;

    // BASINÇ
    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    basincFiltre = tempFloat;

    // İVME X
    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    ivme.x = tempFloat;

    // İVME Y
    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    ivme.y = tempFloat;

    // İVME Z
    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    ivme.z = tempFloat;
    z_ivme = ivme.z;

    // AÇI X
    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    euler.x = tempFloat;

    // AÇI Y
    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    euler.y = tempFloat;

    // AÇI Z
    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    euler.z = tempFloat;

    // KİLİDİ AÇ
    osMutexRelease(SensorMutexHandle);
}
