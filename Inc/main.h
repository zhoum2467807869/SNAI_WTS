/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum
{
	FALSE,
	TRUE,
}BOOL;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern unsigned short Crc_Cal(unsigned short Data, unsigned short GenPoly, unsigned short CrcData);
extern unsigned short CRC_Return(unsigned char *Crc_Buf, unsigned char Crc_Len);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TX2_SCREEN_Pin GPIO_PIN_2
#define TX2_SCREEN_GPIO_Port GPIOA
#define RX2_SCREEN_Pin GPIO_PIN_3
#define RX2_SCREEN_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_5
#define LED2_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_6
#define LED1_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_7
#define KEY1_GPIO_Port GPIOA
#define Active_Low_for_Cloud_rest_Pin GPIO_PIN_2
#define Active_Low_for_Cloud_rest_GPIO_Port GPIOB
#define TX3_The_Cloud_Site_Pin GPIO_PIN_10
#define TX3_The_Cloud_Site_GPIO_Port GPIOB
#define RX3_The_Cloud_Site_D_Pin GPIO_PIN_11
#define RX3_The_Cloud_Site_D_GPIO_Port GPIOB
#define Active_Low_for_1310rest_Pin GPIO_PIN_6
#define Active_Low_for_1310rest_GPIO_Port GPIOC
#define TX1_TO_1310_Pin GPIO_PIN_9
#define TX1_TO_1310_GPIO_Port GPIOA
#define RX1_TO_1310_Pin GPIO_PIN_10
#define RX1_TO_1310_GPIO_Port GPIOA
#define TX4_TO_485_Pin GPIO_PIN_10
#define TX4_TO_485_GPIO_Port GPIOC
#define RX4_TO_485_Pin GPIO_PIN_11
#define RX4_TO_485_GPIO_Port GPIOC
#define SEL_485_Pin GPIO_PIN_12
#define SEL_485_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
