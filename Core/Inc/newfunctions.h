#ifndef NEWFUNCTIONS_H
#define NEWFUNCTIONS_H
#define BNO_ADDR 0x50

typedef enum {
    FAZ_RAMPA, // Rampa konumunda.
	FAZ_FIRLATMA, // İtkili atış kısmı.
    FAZ_TIRMANIS, // Apogee atamaz kilidi var.
	FAZ_ARAYIS, // Kilit kalktı ve apogee atabilir.
    FAZ_DUSUS, // Sürüklenme sonrası durum. Bu fazın sonunda ana paraşütü açacak.
    FAZ_INIS, // Ana paraşüt sonrası alçalma durumu.
	FAZ_BITIS // Yere tam olarak indiğini yahut hızının vs sıfırlandığını belirten durum.
} UcusFazlari;


#include "main.h"
#include <string.h>
#include <stdio.h>
#include "bno055.h"

extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart2;
extern UcusFazlari ucusDurumu;

void veriOkuma(void);
void firlatma(void); // Kalkışın tespitini sağlayan fonksiyon.
void tirmanma(void); // Burnout tespiti yapan fonksiyon.
void arama(void); // Kalkış yapıldıysa apogee aramayı burada başlat.
void drogueAcma(void); // Apogee bulunduysa paraşütü bununla aç.
void anaParasutAcma(void); // Hız istenen duruma geldiyse ana paraşütü bununla aç.
void ledYakma(void); // İniş tamamen yapıldıysa bununla led yak.
void inisKontrol(void); // İnişin başarılı olup olmadığını kontrol eden fonksiyon.
void hizHesaplama(void);
float ortFiltreleme(float ortGuncel, float ortFiltre); // Hareketli ortalama filtresi.
void uartOkuma(void); // UART ile ekranımıza veri gönderen fonksiyon.
#endif
