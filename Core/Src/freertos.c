/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "newfunctions.h"
#include "teknoukb.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
	uint8_t headerByte;
	extern uint8_t header;
	HAL_StatusTypeDef sonuc;
	uint8_t rxBuffer[100];
	extern TestModlari aktifMod;

	uint8_t debug_ab_sayaci = 0;
	uint16_t debug_bekleyen_boyut = 0;
	uint8_t debug_sahteal_sayaci = 0;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for stateMachine */
osThreadId_t stateMachineHandle;
const osThreadAttr_t stateMachine_attributes = {
  .name = "stateMachine",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for sendTele */
osThreadId_t sendTeleHandle;
const osThreadAttr_t sendTele_attributes = {
  .name = "sendTele",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for orderTele */
osThreadId_t orderTeleHandle;
const osThreadAttr_t orderTele_attributes = {
  .name = "orderTele",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for SensorMutex */
osMutexId_t SensorMutexHandle;
const osMutexAttr_t SensorMutex_attributes = {
  .name = "SensorMutex"
};
/* Definitions for TeleSem */
osSemaphoreId_t TeleSemHandle;
const osSemaphoreAttr_t TeleSem_attributes = {
  .name = "TeleSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void stateMachineTask(void *argument);
void sendTeleTask(void *argument);
void orderTeleTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of SensorMutex */
  SensorMutexHandle = osMutexNew(&SensorMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of TeleSem */
  TeleSemHandle = osSemaphoreNew(1, 1, &TeleSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of stateMachine */
  stateMachineHandle = osThreadNew(stateMachineTask, NULL, &stateMachine_attributes);

  /* creation of sendTele */
  sendTeleHandle = osThreadNew(sendTeleTask, NULL, &sendTele_attributes);

  /* creation of orderTele */
  orderTeleHandle = osThreadNew(orderTeleTask, NULL, &orderTele_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {

    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_stateMachineTask */
/**
* @brief Function implementing the stateMachine thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_stateMachineTask */
void stateMachineTask(void *argument)
{
  /* USER CODE BEGIN stateMachineTask */
  /* Infinite loop */
  for(;;)
  {
	  if(aktifMod == MOD_UCUS || aktifMod == MOD_SIT) veriOkuma();
	  else if(aktifMod == MOD_SUT){}


	  hizHesaplama();

	  switch (ucusDurumu){ // Kurtarma algoritması, fonksiyonlar ve switch-case yapısıyla oluşturuldu.
		  	  case FAZ_RAMPA: firlatma(); // Rampadan fırlatmayı tespit etme.
		  	  	  	  	  	  break;
		  	  case FAZ_FIRLATMA: tirmanma(); // Fırlatmadan burnoutu tespit etme.
		  	  	  	  	  	  	 break;
		  	  case FAZ_TIRMANIS: arama(); // Burnout sonrası tepe noktası kilidini açıp aramaya geçme.
		  	  	  	  	  	     break;
		  	  case FAZ_ARAYIS: drogueAcma(); // Drogue açmak için apogee tespiti.
		  		  	  	  	   break;
		  	  case FAZ_DUSUS: anaParasutAcma(); // Ana paraşüt açma tespiti.
		  	  	  	  	  	  break;
			  case FAZ_INIS:  inisKontrol(); // İniş kontrol algoritması ve yedek paraşüt.
			  	  	  	  	  break;
			  case FAZ_BITIS: ledYakma(); // İnişin tamamlanmasıyla beraber yapılacaklar.
			  	  	  	  	  break;
	  }

	  osDelay(80);
  }
  /* USER CODE END stateMachineTask */
}

/* USER CODE BEGIN Header_sendTeleTask */
/**
* @brief Function implementing the sendTele thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_sendTeleTask */
void sendTeleTask(void *argument)
{
  /* USER CODE BEGIN sendTeleTask */
  /* Infinite loop */
  for(;;)
  {
	 if(aktifMod == MOD_UCUS || aktifMod == MOD_SIT) teleGonder();
	 else if(aktifMod == MOD_SUT) fazGonder();
	 osDelay(100);
  }

  /* USER CODE END sendTeleTask */
}

/* USER CODE BEGIN Header_orderTeleTask */
/**
* @brief Function implementing the orderTele thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_orderTeleTask */
void orderTeleTask(void *argument)
{
  /* USER CODE BEGIN orderTeleTask */
	HAL_GPIO_WritePin(LED_PA5_GPIO_Port, LED_PA5_Pin, 1);

    static uint8_t depo[256];
    static uint16_t depoLen = 0;
  /* Infinite loop */
    for(;;)
      {
          osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever); // Görev yalnızca emir gelince uyanır.
          uint16_t rxLen = sizeof(rxBuffer) - __HAL_DMA_GET_COUNTER(huart2.hdmarx); // Gelen verinin boyutunu hesaplama.
          if(depoLen + rxLen > 250) depoLen = 0; // Verilerin tek bir havuzda toplanmasını sağlar.
          memcpy(&depo[depoLen], rxBuffer, rxLen);
          depoLen += rxLen;
          int i = 0;
          while(i < depoLen)
          {
              if(depo[i] == 0xAA){ // Başlığı bulmaya çalışıyoruz buna göre sonraki komutu bekliyoruz.
                  if((depoLen - i) < 5) break;
                  uint8_t gercekCS = depo[i] + depo[i+1];
                  if(depo[i+2] == gercekCS){
                	  modGuncelle(depo[i+1]);
                  }
                  i += 5; // Yuttuk, ilerle
                  continue;
              }
              // SUT
              else if(depo[i] == 0xAB){
                  if((depoLen - i) < 36) break; // 36 bayt veri gelmesini bekleyen fonksiyon.
                  sahteAl(&depo[i]);
                  i += 36;
                  continue;
              }
              else{
                  i++;
              }
          }

          if(i < depoLen) {
              memmove(depo, &depo[i], depoLen - i); // Kesik gelen verileri kaydırıyoruz.
              depoLen = depoLen - i;
          } else {
              depoLen = 0;
          }

          memset(rxBuffer, 0, sizeof(rxBuffer)); // Hafızayı yeni veri alımına hazır hale getiriyoruz.
          HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxBuffer, sizeof(rxBuffer));
      }
  /* USER CODE END orderTeleTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

