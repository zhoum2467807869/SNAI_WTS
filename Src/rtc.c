/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
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

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
current_time_t current_date_time;

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */
/*
*RTC底层驱动，时钟配置
*此函数会被HAL_RTC_Init()调用
*hrtc:RTC句柄
*HAL_PWR_EnableBkUpAccess();//取消备份区域写保护
*__HAL_RCC_PWR_CLK_ENABLE();//使能电源时钟PWR
*/

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  
  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
/* //设置时间 ,根据实际时间修改*/
void RTC_Set_Time(uint8_t Hours,uint8_t Minutes,uint8_t Seconds)
{
    RTC_TimeTypeDef screen_sTime;
    screen_sTime.Hours = Hours;            
    screen_sTime.Minutes = Minutes;          
    screen_sTime.Seconds = Seconds; 
    if(HAL_RTC_SetTime(&hrtc, &screen_sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
      Error_Handler();
    }
}
/*设置日期*/
void RTC_Set_Date(uint8_t WeekDay,uint8_t Month,uint8_t Date,uint8_t Year)
{
  RTC_DateTypeDef screen_sDate;
  screen_sDate.WeekDay = WeekDay; 
  screen_sDate.Month = Month; 
  screen_sDate.Date = Date;  /*Min_Data = 1 and Max_Data = 31 */            
  screen_sDate.Year = Year;    /* Min_Data = 0 and Max_Data = 99 */
                         
  HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,Year);/*备份年月日*/
  HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR2,Month);
  HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR3,Date);
  HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR4,WeekDay);
  if(HAL_RTC_SetDate(&hrtc, &screen_sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}

/*恢复日期*/
void RTC_MY_recoverDate(void)
{
    RTC_DateTypeDef screen_sDate;
	screen_sDate.Year = current_date_time.year = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1);
	screen_sDate.Month = current_date_time.month = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR2);
	screen_sDate.Date = current_date_time.day = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR3);
    screen_sDate.WeekDay = current_date_time.WeekDay = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR4);
    HAL_RTC_SetDate(&hrtc, &screen_sDate, RTC_FORMAT_BIN);//重新写入使其运行
}

/*获取时间*/
int Get_Date_Time(void)
{
    RTC_DateTypeDef screen_sDate;
    RTC_TimeTypeDef screen_sTime;
	
    if(HAL_OK   == HAL_RTC_GetTime(&hrtc, &screen_sTime, RTC_FORMAT_BIN)
            && HAL_OK == HAL_RTC_GetDate(&hrtc, &screen_sDate, RTC_FORMAT_BIN))
    {
        current_date_time.year = screen_sDate.Year+2000;
        current_date_time.month = screen_sDate.Month;
        current_date_time.day = screen_sDate.Date;
        current_date_time.hour =  screen_sTime.Hours;
        current_date_time.minute  =  screen_sTime.Minutes;
        current_date_time.WeekDay = screen_sDate.WeekDay;
        current_date_time.Seconds = screen_sTime.Seconds;
        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,screen_sDate.Year);/*备份年月日*/
        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR2,screen_sDate.Month);
        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR3,screen_sDate.Date);
        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR4,screen_sDate.WeekDay);
        return 0 ;
    }

    return -1 ;
}


/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
