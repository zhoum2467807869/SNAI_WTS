/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "iwdg.h"
#include "screen_com.h"
#include "cc1310_com.h"
#include "rs485_opt.h"
#include "CircularQueue.h"
#include "rtc.h"
#include "spi.h"
#include "flash_opt.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define W25Q16_CS_LOW()       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
#define W25Q16_CS_HIGH()      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
  SemaphoreHandle_t xSemaphore = NULL;      /* �����ź������ */


/* USER CODE END Variables */
osThreadId IWDGHandle;
osThreadId SCREEN_OPTHandle;
osThreadId SX1310_OPTHandle;
osThreadId UPDATE_CLOUDHandle;
osThreadId RS485_OPTHandle;
osThreadId RTC_UPDATEHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void get_resert_status(void); 
/* USER CODE END FunctionPrototypes */

void Start_IWDG(void const * argument);
void Start_Screen_Opt(void const * argument);
void Start_SX1310_Opt(void const * argument);
void Start_Update_Cloud(void const * argument);
void Start_RS485_Opt(void const * argument);
void Start_RTC_Update(void const * argument);

extern void MX_USB_DEVICE_Init(void);
extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
    
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */

  xSemaphore = xSemaphoreCreateMutex();   /* ���������ź��� */
  /* USER CODE END RTOS_MUTEX */

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
  /* definition and creation of IWDG */
  osThreadDef(IWDG, Start_IWDG, osPriorityHigh, 0, 128);
  IWDGHandle = osThreadCreate(osThread(IWDG), NULL);

  /* definition and creation of SCREEN_OPT */
  osThreadDef(SCREEN_OPT, Start_Screen_Opt, osPriorityRealtime, 0, 128);
  SCREEN_OPTHandle = osThreadCreate(osThread(SCREEN_OPT), NULL);

  /* definition and creation of SX1310_OPT */
  osThreadDef(SX1310_OPT, Start_SX1310_Opt, osPriorityIdle, 0, 128);
  SX1310_OPTHandle = osThreadCreate(osThread(SX1310_OPT), NULL);

  /* definition and creation of UPDATE_CLOUD */
  osThreadDef(UPDATE_CLOUD, Start_Update_Cloud, osPriorityIdle, 0, 128);
  UPDATE_CLOUDHandle = osThreadCreate(osThread(UPDATE_CLOUD), NULL);

  /* definition and creation of RS485_OPT */
  osThreadDef(RS485_OPT, Start_RS485_Opt, osPriorityIdle, 0, 128);
  RS485_OPTHandle = osThreadCreate(osThread(RS485_OPT), NULL);

  /* definition and creation of RTC_UPDATE */
  osThreadDef(RTC_UPDATE, Start_RTC_Update, osPriorityIdle, 0, 128);
  RTC_UPDATEHandle = osThreadCreate(osThread(RTC_UPDATE), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Start_IWDG */
/**
  * @brief  Function implementing the IWDG thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_Start_IWDG */
void Start_IWDG(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* init code for FATFS */
  MX_FATFS_Init();
  /* USER CODE BEGIN Start_IWDG */
  /* Infinite loop */
  for(;;)
  {
    //HAL_IWDG_Refresh(&hiwdg);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);//RED-LED IS RUNNING
    osDelay(2000);
    if(0 == HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7))//��ȡ��λ����active low for reset
    {
      /*��SYSRESETREQ ����Ϊ��Ч������λ������ִ�и�λ�����������һ����ʱ��
        �ڴ���ʱ�ڼ䣬��������Ȼ������Ӧ�ж����󡣵����ǵı���������Ҫ�ô˴�ִ�е���Ϊֹ��
        ��Ҫ�����κ����������ˡ����ԣ�����ڷ�����λ����ǰ���Ȱ�FAULTMASK ��λ��*/
      __set_FAULTMASK(1); //�ر������ж�
      get_resert_status();
      NVIC_SystemReset();
    }
  }
  /* USER CODE END Start_IWDG */
}

/* USER CODE BEGIN Header_Start_Screen_Opt */
/**
* @brief Function implementing the SCREEN_OPT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_Screen_Opt */
void Start_Screen_Opt(void const * argument)
{
  /* USER CODE BEGIN Start_Screen_Opt */
  /* Infinite loop */
  CQ_init( &screen_circ,(uint8_t *)screen_rx_buff, SCREEN_RX_BUF_MAX);
  
  uint8_t mcuid_lock = 0;
  for(;;)
  {
      xSemaphoreTake( xSemaphore, portMAX_DELAY );
	  screen_main_opt();//��ȡ����ָ��    
      switch(screen_pageid_CURRENT)
      {
        case MAIN_SCREEN:
          mcuid_lock = 0;
          //Get_Date_Time();/*�����߳�1s����*/
          break;
        case MENU_SCREEN:
          mcuid_lock = 0;
          break;
        case NODE1_SCREEN:
          mcuid_lock = 0;
          break;
        case NODE2_SCREEN:
          mcuid_lock = 0;
          break;
        case NODE3_SCREEN:
          mcuid_lock = 0;
          break;
        case NODE4_SCREEN:
          mcuid_lock = 0;
          break;
        case ALMSET_SCREEN:
          mcuid_lock = 0;
          break;  
        case PROPERTY_SCREEN:
          if(mcuid_lock == 0)
          {
            Get_MCUUID();
            mcuid_lock = 1;
          }
          break;
        case ALARM_SCREEN:
          mcuid_lock = 0;
          break;
        case CLOCK_SCREEN:
          mcuid_lock = 0;
          break;
        default:
          NVIC_SystemReset();/*���ϸ�λ*/
          break;
      }
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);//GREEN-LED IS RUNNING
      xSemaphoreGive( xSemaphore );
	  osDelay(200);
      
  }
  /* USER CODE END Start_Screen_Opt */
}

/* USER CODE BEGIN Header_Start_SX1310_Opt */
/**
* @brief Function implementing the SX1310_OPT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_SX1310_Opt */
void Start_SX1310_Opt(void const * argument)
{
  /* USER CODE BEGIN Start_SX1310_Opt */
  CC1310_Opt_init();
  CQ_init( &cc1310_circ,(uint8_t *)CC1310_RX_BUF, CC1310_RX_BUF_MAX);
  /* Infinite loop */
  for(;;)
  {
    xSemaphoreTake( xSemaphore, portMAX_DELAY );
    CC1310_Opt_Loop();
    xSemaphoreGive( xSemaphore );
    osDelay(200); 
  }
  /* USER CODE END Start_SX1310_Opt */
}

/* USER CODE BEGIN Header_Start_Update_Cloud */
/**
* @brief Function implementing the UPDATE_CLOUD thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_Update_Cloud */
void Start_Update_Cloud(void const * argument)
{
  /* USER CODE BEGIN Start_Update_Cloud */
  /* Infinite loop */
  for(;;)
  {   
    osDelay(1);
  }
  /* USER CODE END Start_Update_Cloud */
}

/* USER CODE BEGIN Header_Start_RS485_Opt */
/**
* @brief Function implementing the RS485_OPT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_RS485_Opt */
void Start_RS485_Opt(void const * argument)
{
  /* USER CODE BEGIN Start_RS485_Opt */
  //SPI_Flash_Wait_Busy();
//  W25Q16_Init(); 
//  uint8_t para[40] = {"hello,world!"};
//  para[0] = 0xEE;
//  uint8_t read_buf[40] = {0};
//  SPI_Flash_Wait_Busy();
//  SPI_FLASH_SectorErase(0x0U);
//  printf("�����С�����\r");
//  SPI_FLASH_BufferWrite((uint8_t*) para, 0x0U,40);
//  W25Q16_CS_HIGH();
//  printf("��д�롣����\r");
//  W25Q16_CS_LOW();
//  SPI_Flash_Read((uint8_t*) read_buf, 0x0U,40);
//  printf("������%02x\r",read_buf[0]);  
  
  /* Infinite loop */
  for(;;)
  {
    //SPI_Flash_ReadID();
    osDelay(200);
  }
  /* USER CODE END Start_RS485_Opt */
}

/* USER CODE BEGIN Header_Start_RTC_Update */
/**
* @brief Function implementing the RTC_UPDATE thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_RTC_Update */
void Start_RTC_Update(void const * argument)
{
  /* USER CODE BEGIN Start_RTC_Update */
  /* Infinite loop */
  /*
 uint16_t temp=GPIOx->IDR;//��16��IO��temp     �üĴ�����so easy�ˡ�

*/
  char current_time[64] = {0};
  for(;;)
  {
    if(screen_pageid_CURRENT == MAIN_SCREEN)
    {
      Get_Date_Time();/*����1s��ȡ*/
//      switch(current_date_time.WeekDay)
//      {
//      case 1:
//        sprintf(screen_weekday,"һ");
//        break;
//      case 2:
//        sprintf(screen_weekday,"��");
//        break;
//      case 3:
//        sprintf(screen_weekday,"��");
//        break;  
//      case 4:
//        sprintf(screen_weekday,"��");
//        break;
//      case 5:
//        sprintf(screen_weekday,"��");
//        break;
//      case 6:
//        sprintf(screen_weekday,"��");
//        break;
//      case 0:
//        sprintf(screen_weekday,"��");
//        break;
//      default:
//        break;
//      }
      sprintf(current_time,"g0.txt=\"%u-%02u-%02u %02u:%02u:%02u\"",current_date_time.year,current_date_time.month,current_date_time.day,current_date_time.hour,current_date_time.minute,current_date_time.Seconds);
//      HAL_UART_Transmit_DMA(&huart2,(uint8_t *)current_time,strlen(current_time));
//      osDelay(4);
//      HAL_UART_Transmit_DMA(&huart2,(uint8_t*)screen_EOF_TxBuf,strlen((char const*)screen_EOF_TxBuf));
      HAL_UART_Transmit(&huart2,(uint8_t*)current_time,strlen(current_time),0x400);//��������ʽѡ�����ʱ����֤һ��ʱ���ڷ��ͳ�ȥ
      HAL_UART_Transmit(&huart2,(uint8_t*)screen_EOF_TxBuf,strlen((char const*)screen_EOF_TxBuf),0x400);
      osDelay(1000);
    }
  }
  /* USER CODE END Start_RTC_Update */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
#ifndef LPWRRSTF
#define LPWRRSTF 0x80000000U/*31λ�͹��ĸ�λ��־*/
#endif
#ifndef WWDGRSTF
#define WWDGRSTF 0x40000000U/*30λ���ڿ��Ź���λ��־*/
#endif
#ifndef IWDGRSTF
#define IWDGRSTF 0x20000000U/*29λ�������Ź���λ��־*/
#endif
#ifndef SFTRSTF
#define SFTRSTF  0x10000000U/*28λ�����λ��־*/
#endif
#ifndef PORRSTF
#define PORRSTF  0x08000000U/*27λ�ϵ�/���縴λ��־*/
#endif
#ifndef PINRSTF
#define PINRSTF  0x04000000U/*26λNRST���Ÿ�λ��־*/
#endif
#ifndef NOTHING
#define NOTHING  0x02000000U/*25λ����������������0*/
#endif
#ifndef RMVF
#define RMVF     0x01000000U/*24λ�����λ��־*/
#endif

void get_resert_status(void)
{
                                                                                //     if(__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST)) //ѡ���ֽ����ظ�λ
                                                                                //     printf("RCC_FLAG_OBLRST reset");
  
  
  /***************************************************�ο�STM32���Ĳο��ֲ�7.1�½ڸ�λ******************************************/
//      if( READ_BIT(RCC->CSR, RCC_CSR_FWRSTF) != RESET)
//    {
//        HAL_UART_Transmit(&huart2,"Ӳ��ԭ��λ\n",strlen("Ӳ��ԭ��λ\n"),0x400);
//    }
//    if( READ_BIT(RCC->CSR, RCC_CSR_OBLRSTF) != RESET)
//    {
//        HAL_UART_Transmit(&huart2,"�����ֽڸ�λ\n",strlen("�����ֽڸ�λ\n"),0x400);
//    }
    if( READ_BIT(RCC->CSR, PINRSTF) != RESET)
    {
        HAL_UART_Transmit(&huart2,"��λ������λ\n",strlen("��λ������λ\n"),0x400);
    }
    if( READ_BIT(RCC->CSR, PORRSTF) != RESET)
    {
        HAL_UART_Transmit(&huart2,"���ڵ�ѹ��ֵ��λ\n",strlen("���ڵ�ѹ��ֵ��λ\n"),0x400);
    }
    if( READ_BIT(RCC->CSR, SFTRSTF) != RESET)
    {
        HAL_UART_Transmit(&huart2,"�����λ\n",strlen("�����λ\n"),0x400);
    }
    if( READ_BIT(RCC->CSR, IWDGRSTF) != RESET)
    {
        HAL_UART_Transmit(&huart2,"���ÿ��Ź���λ\n",strlen("���ÿ��Ź���λ\n"),0x400);
    }
    if( READ_BIT(RCC->CSR, WWDGRSTF) != RESET)
    {
        HAL_UART_Transmit(&huart2,"���ÿ��Ź���λ\n",strlen("���ÿ��Ź���λ\n"),0x400);
    }
    if( READ_BIT(RCC->CSR, LPWRRSTF) != RESET)
    {
        HAL_UART_Transmit(&huart2,"�͹��ķǷ�ģʽ��λ\n",strlen("�͹��ķǷ�ģʽ��λ\n"),0x400);
    }
  /********************************************************************************************************/

//    if( READ_BIT(RCC->CSR, RCC_CSR_FWRSTF) != RESET)
//    {
//        HAL_UART_Transmit(&huart2,"Ӳ��ԭ��λ\n",strlen("Ӳ��ԭ��λ\n"),0x400);
//    }
//    if( READ_BIT(RCC->CSR, RCC_CSR_OBLRSTF) != RESET)
//    {
//        HAL_UART_Transmit(&huart2,"�����ֽڸ�λ\n",strlen("�����ֽڸ�λ\n"),0x400);
//    }
//    if( READ_BIT(RCC->CSR, RCC_CSR_PINRSTF) != RESET)
//    {
//        HAL_UART_Transmit(&huart2,"��λ������λ\n",strlen("��λ������λ\n"),0x400);
//    }
//    if( READ_BIT(RCC->CSR, RCC_CSR_BORRSTF) != RESET)
//    {
//        HAL_UART_Transmit(&huart2,"���ڵ�ѹ��ֵ��λ\n",strlen("���ڵ�ѹ��ֵ��λ\n"),0x400);
//    }
//    if( READ_BIT(RCC->CSR, RCC_CSR_SFTRSTF) != RESET)
//    {
//        HAL_UART_Transmit(&huart2,"�����λ\n",strlen("�����λ\n"),0x400);
//    }
//    if( READ_BIT(RCC->CSR, RCC_CSR_IWDGRSTF) != RESET)
//    {
//        HAL_UART_Transmit(&huart2,"���ÿ��Ź���λ\n",strlen("���ÿ��Ź���λ\n"),0x400);
//    }
//    if( READ_BIT(RCC->CSR, RCC_CSR_WWDGRSTF) != RESET)
//    {
//        HAL_UART_Transmit(&huart2,"���ÿ��Ź���λ\n",strlen("���ÿ��Ź���λ\n"),0x400);
//    }
//    if( READ_BIT(RCC->CSR, RCC_CSR_LPWRRSTF) != RESET)
//    {
//        HAL_UART_Transmit(&huart2,"�͹��ķǷ�ģʽ��λ\n",strlen("�͹��ķǷ�ģʽ��λ\n"),0x400);
//    }
    __HAL_RCC_CLEAR_RESET_FLAGS();
}
 
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
