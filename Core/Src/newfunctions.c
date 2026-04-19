#include "newfunctions.h"

UcusFazlari ucusDurumu = FAZ_RAMPA;

float z_ivme = 0, dikey_hiz = 0.0f;
uint32_t gecmis_zaman = 0, dusus_sayaci = 0;

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
	euler = bno055_getVectorEuler();
	ivme = bno055_getVectorAccelerometer();
	z_ivme = ortFiltreleme(ivme.z,z_ivme);
}

void firlatma(void){
	if(z_ivme > 20.0f){
		ucusDurumu = FAZ_FIRLATMA;
	}
}

void tirmanma(void){
	    if (z_ivme < 2.0f) {
	        ucusDurumu = FAZ_TIRMANIS;
	    }
}

void arama(void){
	if(dikey_hiz > 30.0f){
		ucusDurumu = FAZ_ARAYIS;
	}
}

void drogueAcma(void){
	if(dikey_hiz < -1.0f){
		dusus_sayaci++;

		if(dusus_sayaci > 5){
			HAL_GPIO_WritePin(TEPE_PA9_GPIO_Port, TEPE_PA9_Pin, 1); // Drogue paraşütü servosu çalıştı paraşüt atıldı.
			HAL_GPIO_WritePin(LED_PA4_GPIO_Port, LED_PA4_Pin, 1);
			ucusDurumu = FAZ_DUSUS;
	}

	}else if((euler.y > 70.0f || euler.y < -70.0f || euler.x > 70.0f || euler.x < -70.0f) && (dikey_hiz < 15.0f)){
		HAL_GPIO_WritePin(TEPE_PA9_GPIO_Port, TEPE_PA9_Pin, 1); // Drogue paraşütü servosu çalıştı paraşüt atıldı.
		HAL_GPIO_WritePin(LED_PA4_GPIO_Port, LED_PA4_Pin, 1);
		ucusDurumu = FAZ_DUSUS;

	}else{
		dusus_sayaci=0;
	}

}

void hizHesaplama(float z_ivme) {

    uint32_t guncel_zaman = HAL_GetTick();
    float dt = (guncel_zaman - gecmis_zaman) / 1000.0f;
    gecmis_zaman = guncel_zaman;
    if (dt > 0.5f) return;

    dikey_hiz += (z_ivme * dt);
}

float ortFiltreleme(float ortGuncel, float ortFiltre) {
		ortFiltre = (ortGuncel * 0.1) + (ortFiltre * 0.9);
	return ortFiltre;
}

void uartOkuma(void){ // UART'a veri aktarmamızı sağlayan fonksiyon bu sayede test yaparken ekranda bu büyüklükleri göreceğiz.
	char buffer[100];
	sprintf(buffer, "Euler-> Yaw: %6.1f | Roll: %6.1f | Pitch: %6.1f || Ivme-> X: %6.2f | Y: %6.2f | Z: %6.2f\r\n",
	              euler.x, euler.y, euler.z,
	              ivme.x, ivme.y, ivme.z);
	HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 100);
}
