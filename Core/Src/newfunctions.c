#include "newfunctions.h"

UcusFazlari ucusDurumu = FAZ_RAMPA;
// SUT testindeki ışıkları yakmak için durum kullandık.
uint16_t durum;

float z_ivme = 0, dikey_hiz = 0.0f;
float irtifaBagil = 0, irtifaMax = 0, irtifaFiltre = 0, irtifaGuncel = 0,
		irtifaBaslangic = 0, basincFiltre = 0, basincGuncel = 0, basincBaslangic = 0,
		sicaklikFiltre = 0, sicaklikGuncel = 0;
uint32_t gecmis_zaman = 0, durma_zamani = 0, dusus_sayaci = 0;
bno055_vector_t euler;
bno055_vector_t ivme;

void bno055_writeData(uint8_t reg, uint8_t data) {
    HAL_I2C_Mem_Write(&hi2c2, BNO_ADDR, reg, 1, &data, 1, 10);
}

void bno055_readData(uint8_t reg, uint8_t *data, uint8_t len) {
    HAL_I2C_Mem_Read(&hi2c2, BNO_ADDR, reg, 1, data, len, 100);
}

void bno055_delay(int time) {
    HAL_Delay(time);
}

void veriOkuma(void){

	osMutexAcquire(SensorMutexHandle, osWaitForever);

	euler = bno055_getVectorEuler();
	ivme = bno055_getVectorLinearAccel();
	basincGuncel = BMP180_GetPressure() / 100.0f;
	sicaklikGuncel = BMP180_GetTemperature();

	if(basincBaslangic == 0.0f && basincGuncel > 0.0f) {
		    basincBaslangic = basincGuncel;
	        basincFiltre = basincGuncel;
	        sicaklikFiltre = sicaklikGuncel;
	        z_ivme = ivme.z;
		}

	basincFiltre = ortFiltreleme(basincGuncel, basincFiltre);
	sicaklikFiltre = ortFiltreleme(sicaklikGuncel, sicaklikFiltre);
	z_ivme = ortFiltreleme(ivme.z,z_ivme);

	if (basincBaslangic > 0) irtifaGuncel = irtifaHesaplama();
	irtifaFiltre = ortFiltreleme(irtifaGuncel, irtifaFiltre);
	irtifaBagil = irtifaFiltre - irtifaBaslangic;
	if (irtifaBagil < 0) irtifaBagil = 0;

	osMutexRelease(SensorMutexHandle);
}

void firlatma(void){
	if(irtifaBaslangic == 0 && irtifaFiltre > 0){
	        irtifaBaslangic = irtifaFiltre;
	    }
	if(z_ivme > 15.0f){
		durum |= (1 << 0);
		ucusDurumu = FAZ_FIRLATMA;
	}
}

void tirmanma(void){
	    if(z_ivme < 20.0f){
	    	durum |= (1 << 1);
	    	ucusDurumu = FAZ_TIRMANIS;
	    }
}

void arama(void){
    if(irtifaBagil > 2000.0f) {
        durum |= (1 << 2);
    }
    if(euler.y > 40.0f || euler.y < -40.0f || euler.x > 40.0f || euler.x < -40.0f) {
        durum |= (1 << 3);
    }
    if(dikey_hiz <= -0.0f){
    	dusus_sayaci++;
    	if(dusus_sayaci > 4){
            durum |= (1 << 4);
            ucusDurumu = FAZ_ARAYIS;
    	}
    }else{
    	dusus_sayaci = 0;
    }
}

void drogueAcma(void){
        HAL_GPIO_WritePin(TEPE_PA9_GPIO_Port, TEPE_PA9_Pin, 1);
        HAL_GPIO_WritePin(LED_PA4_GPIO_Port, LED_PA4_Pin, 1);
        durum |= (1 << 5);
        ucusDurumu = FAZ_DUSUS;
}

void anaParasutAcma(void){
	if(irtifaFiltre < 450.0f && dikey_hiz < -3.0f){
		durum |= (1 << 6);
		HAL_GPIO_WritePin(ANA_PA8_GPIO_Port, ANA_PA8_Pin, 1);
		HAL_GPIO_WritePin(LED_PA6_GPIO_Port, LED_PA6_Pin, 1);
		durum |= (1 << 7);
	}
}

void inisKontrol(void) {
    if (dikey_hiz < -50.0f) {
        HAL_GPIO_WritePin(YEDEK_PA7_GPIO_Port, YEDEK_PA7_Pin, 1);
    }
    if ((z_ivme < 0.5f && z_ivme > -0.5f) && (dikey_hiz < 1.0f && dikey_hiz > -1.0f)) {

    	if(durma_zamani == 0){
    		durma_zamani = HAL_GetTick();
    	}
    	if((HAL_GetTick() - durma_zamani) > 5000){
        ucusDurumu = FAZ_BITIS;
    	}// İnişin doğruluğunu kontrol edip yedek paraşüt açan fonksiyon.
    }else{
		durma_zamani = 0;
	}
}

void ledYakma(void) {
		HAL_GPIO_TogglePin(LED_PA6_GPIO_Port, LED_PA6_Pin); // İnince led yakma.
}

void hizHesaplama(void) {
    uint32_t guncel_zaman = HAL_GetTick();
    float dt = (guncel_zaman - gecmis_zaman) / 1000.0f;

    if (dt >= 0.1f) {
        static float irtifa_eski = 0.0f;

        if (irtifa_eski == 0.0f && irtifaFiltre > 0.0f) {
            irtifa_eski = irtifaFiltre;
        }

        dikey_hiz = (irtifaFiltre - irtifa_eski) / dt;

        irtifa_eski = irtifaFiltre;
        gecmis_zaman = guncel_zaman;
    }
}

float irtifaHesaplama(void) {
	irtifaGuncel = 44330.0f * (1.0f - powf(basincFiltre / DENIZ_BAS, 0.1903f));
    return irtifaGuncel; // Basınç ve sıcaklıktan irtifa hesabı formülü.
}

float ortFiltreleme(float ortGuncel, float ortFiltre) {
		ortFiltre = (ortGuncel * 0.1) + (ortFiltre * 0.9);
	return ortFiltre;
}

void uartOkuma(void){ // UART'a veri aktarmamızı sağlayan fonksiyon bu sayede test yaparken ekranda bu büyüklükleri göreceğiz.
	char buffer[200];
	sprintf(buffer, "Euler-> Yaw: %6.1f | Roll: %6.1f | Pitch: %6.1f || Ivme-> X: %6.2f | Y: %6.2f | Z: %6.2f || Faz: %d\r\n",
	              euler.x, euler.y, euler.z,
	              ivme.x, ivme.y, ivme.z, ucusDurumu);
	HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 100);
}
