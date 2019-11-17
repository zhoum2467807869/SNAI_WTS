/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "fatfs.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "screen_com.h"
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

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_UART4_Init();
  MX_SPI2_Init();
  //MX_IWDG_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
    /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* init code for FATFS */
  MX_FATFS_Init();
  
  if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR5) != 0X6066)//是否第一次配置
  { 
    RTC_Set_Time(9,27,40);	        /*设置时间 ,根据实际时间修改*/
    RTC_Set_Date(0,10,27,19);	/*设置日期*/
    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR5,0X6066);//标记已经初始化过了
  }
  else
  {
    RTC_MY_recoverDate();/*从后备区恢复日期*/
  }
  __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);   //打开空闲中断
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)UART1_BUFF, UART1_BUFF_MAX);
  __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);   //打开空闲中断
  //__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE); //打开接收中断
    HAL_UART_Receive_DMA(&huart2, (uint8_t*)UART2_BUFF, UART2_BUFF_MAX);
  __HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);   //打开空闲中断
    HAL_UART_Receive_DMA(&huart3, (uint8_t*)UART3_BUFF, UART3_BUFF_MAX);
  __HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);   //打开空闲中断  
    HAL_UART_Receive_IT(&huart4,  (uint8_t*)UART4_BUFF, UART4_BUFF_MAX);
//   	while(f_mount(&USERFatFS, "0:", 1));
//	while( f_open(&USERFile, "0:123.txt", FA_OPEN_ALWAYS  | FA_WRITE));
//	while(f_write(&USERFile, fat, 6, &br)); 
//	f_close(&USERFile); 
    
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init(); 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USB;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System 
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
/*CRC16 效验*/
unsigned short Crc_Cal(unsigned short Data, unsigned short GenPoly, unsigned short CrcData) 
{
	unsigned short TmpI;
	Data *= 2;
	for (TmpI = 8; TmpI > 0; TmpI--) {
		Data = Data / 2;
		if ((Data ^ CrcData) & 1)
			CrcData = (CrcData / 2) ^ GenPoly;
		else
			CrcData /= 2;
	}
	return CrcData;
}

unsigned short CRC_Return(unsigned char *Crc_Buf, unsigned char Crc_Len)
{
        unsigned short temp;
        unsigned short CRC_R = 0xFFFF;
        for (temp = 0; temp < Crc_Len; temp++)
        {
                CRC_R = Crc_Cal(Crc_Buf[temp], 0xA001, CRC_R);
        }
        return CRC_R;
}
/*
参考
STM32F030F4P6硬件配置： FLASH (16KB) RAM (4KB)
（包含4个扇区，1个扇区包含4个页，每页有1Kbyte空间）
*/
//  static FLASH_EraseInitTypeDef EraseInitStruct = {
//	.TypeErase = FLASH_TYPEERASE_PAGES,       //页擦除
//	.PageAddress = 0x0800F000,                //擦除地址
//	.NbPages = 1                              //擦除页数
//  };
//
//  HAL_FLASH_Unlock();
//  uint32_t PageError = 0;
//  __disable_irq();                             //擦除前关闭中断
//  if (HAL_FLASHEx_Erase(&EraseInitStruct,&PageError) == HAL_OK)
//  {
//      printf("擦除 成功\r\n");
//  }
//  __enable_irq();                             //擦除后打开中断
//  uint32_t writeFlashData = 0x55555555;        //待写入的值
//  uint32_t addr = 0x0800F000;                  //写入的地址
//  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,addr, writeFlashData);
//  printf("at address:0x%x, read value:0x%x\r\n", addr, *(__IO uint32_t*)addr);
//  HAL_FLASH_Lock();
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
