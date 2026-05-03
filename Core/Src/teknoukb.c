#include "teknoukb.h"
 // Başka dosyalarda tanımlanan ve kullanmamız gereken değişkenleri tanımlıyoruz.
extern UART_HandleTypeDef huart2;
extern bno055_vector_t ivme;
extern bno055_vector_t euler;
extern float irtifaFiltre;
extern float basincFiltre;
// Aktif modu standart olarak uçuş seçtik.
TestModlari aktifMod = MOD_UCUS;
// SUT testindeki ışıkları yakmak için durum kullandık.
uint16_t durum;
uint8_t telePaket[36] = {0}; // Dışarıdan alacağımız global paketler.
uint8_t fazPaket[6] = {0};

uint8_t header = 0xAB; // Header ve footerları direkt tanımladık.
uint8_t footer1 = 0x0D;
uint8_t footer2 = 0x0A;

void modGuncelle(uint8_t komut){
	if(komut == 0x20) aktifMod = MOD_SIT; // Komuta göre mod güncelleme.
	else if(komut == 0x22) aktifMod = MOD_SUT;
	else if(komut == 0x24) aktifMod = MOD_UCUS;
}

void durumGuncelle(void) {
    durum = 0;

    if (ucusDurumu >= FAZ_FIRLATMA) durum |= (1 << 0); // Durumları değiştirme fonksiyonu.
    if (ucusDurumu >= FAZ_TIRMANIS) durum |= (1 << 1);
    if (ucusDurumu >= FAZ_ARAYIS)   durum |= (1 << 4);

    if (HAL_GPIO_ReadPin(TEPE_PA9_GPIO_Port, TEPE_PA9_Pin) == GPIO_PIN_SET){
        durum |= (1 << 5);
    }
    if (HAL_GPIO_ReadPin(ANA_PA8_GPIO_Port, ANA_PA8_Pin) == GPIO_PIN_SET){
        durum |= (1 << 7);
    }
}

void teleGonder(void){
	FloatDonusturucu cevirici;
	uint32_t teleChecksumToplam = 0;
	uint8_t teleChecksum = 0;
	uint8_t index = 0;
// Değişkenlerimizi mecburen fonksiyon içinde seçtik hepsi kendi fonksiyonuna özel.
	telePaket[index++] = header;
// Union yöntemiyle burada çevirmeyi yapıyoruz.
	cevirici.giren = irtifaFiltre;
	telePaket[index++] = cevirici.cikan[3];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[0];

	cevirici.giren = basincFiltre;
	telePaket[index++] = cevirici.cikan[3];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[0];

	cevirici.giren = ivme.x;
	telePaket[index++] = cevirici.cikan[3];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[0];

	cevirici.giren = ivme.y;
	telePaket[index++] = cevirici.cikan[3];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[0];

	cevirici.giren = ivme.z;
	telePaket[index++] = cevirici.cikan[3];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[0];

	cevirici.giren = euler.x;
	telePaket[index++] = cevirici.cikan[3];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[0];

	cevirici.giren = euler.y;
	telePaket[index++] = cevirici.cikan[3];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[0];

	cevirici.giren = euler.z;
	telePaket[index++] = cevirici.cikan[3];
	telePaket[index++] = cevirici.cikan[2];
	telePaket[index++] = cevirici.cikan[1];
	telePaket[index++] = cevirici.cikan[0];

	for(int i=0;i<index;i++){
		teleChecksumToplam += telePaket[i];
	}
	// Checksum gerçekten gönderilecek veriye göre hesaplanıyor.
	teleChecksum = teleChecksumToplam % 256;
	telePaket[index++] = teleChecksum;

	telePaket[index++] = footer1;
	telePaket[index++] = footer2;

	HAL_UART_Transmit(&huart2, telePaket, sizeof(telePaket), 100);
}

void fazGonder(void){
    durumGuncelle();
    uint8_t index = 0;
// Güncellenen durumları göndermemizi sağlayan fonksiyon.
    fazPaket[index++] = 0xAA;
    fazPaket[index++] = (uint8_t)(durum & 0xFF);
    fazPaket[index++] = (uint8_t)((durum >> 8) & 0xFF);

    uint32_t fazChecksumToplam = fazPaket[0] + fazPaket[1] + fazPaket[2];
    fazPaket[index++] = fazChecksumToplam % 256;
    // Checksum gerçekten gönderilecek veriye göre hesaplanıyor.
    fazPaket[index++] = footer1; // 0x0D
    fazPaket[index++] = footer2; // 0x0A

    HAL_UART_Transmit(&huart2, fazPaket, 6, 100);
}

void sahteAl(uint8_t *sahteVeri){
    uint8_t temp[4]; // Tek tek paketler halinde almamızı sağlıyor.
    float tempFloat; // Paketleri direkt çevirmemizi sağlıyor.
    uint8_t index = 1; // Headerı ayrıca aldığımız için 0 yerine 1'den başlıyor.

    osMutexAcquire(SensorMutexHandle, osWaitForever);
// Aynı I2C hattını kullandığımız için mutex kullanmak zorundayız veri güvenliği için.
    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4); // Alınan veriyi direkt istasyo
    irtifaFiltre = tempFloat; // Double türündeki veriyi direkt memcpy ile gönderemeyiz.

    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    basincFiltre = tempFloat;

    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    ivme.x = tempFloat;

    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    ivme.y = tempFloat;

    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    ivme.z = tempFloat;
    z_ivme = ivme.z;

    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    euler.x = tempFloat;

    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    euler.y = tempFloat;

    temp[3] = sahteVeri[index++];
    temp[2] = sahteVeri[index++];
    temp[1] = sahteVeri[index++];
    temp[0] = sahteVeri[index++];
    memcpy(&tempFloat, temp, 4);
    euler.z = tempFloat;

    osMutexRelease(SensorMutexHandle);
}
