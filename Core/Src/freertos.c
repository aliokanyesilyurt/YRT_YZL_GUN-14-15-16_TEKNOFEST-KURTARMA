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
#include "main.h"
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
	uint8_t rxBuffer[2];
	extern uint8_t header;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for stateMachine */
osThreadId_t stateMachineHandle;
const osThreadAttr_t stateMachine_attributes = {
  .name = "stateMachine",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
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
  .stack_size = 128 * 4,
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
	  HAL_GPIO_TogglePin(LED_PA5_GPIO_Port, LED_PA5_Pin);
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
	  else if(aktifMod == MOD_SUT){

	  }
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
  /* Infinite loop */
  for(;;)
  {
	  if(osSemaphoreAcquire(TeleSemHandle, osWaitForever) == osOK){
		  if(rxBuffer[0] == header){
			  modGuncelle(rxBuffer[1]);
		  }
		  HAL_UART_Receive_IT(&huart2, rxBuffer, 2);
	  }
    osDelay(1);
  }
  /* USER CODE END orderTeleTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

