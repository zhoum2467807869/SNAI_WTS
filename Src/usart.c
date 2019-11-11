/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "stm32f1xx_hal.h"
#include "CircularQueue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char UART1_BUFF[UART1_BUFF_MAX];/*cc1310*/
char UART2_BUFF[UART2_BUFF_MAX];/*screen*/
char UART3_BUFF[UART3_BUFF_MAX];/*Cloud*/
char UART4_BUFF[UART4_BUFF_MAX];/*RS485*/
/* USER CODE END 0 */

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* UART4 init function */
void MX_UART4_Init(void)
{

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */
    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();
  
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**UART4 GPIO Configuration    
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX 
    */
    GPIO_InitStruct.Pin = TX4_TO_485_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TX4_TO_485_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RX4_TO_485_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(RX4_TO_485_GPIO_Port, &GPIO_InitStruct);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = TX1_TO_1310_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TX1_TO_1310_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RX1_TO_1310_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(RX1_TO_1310_GPIO_Port, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = TX2_SCREEN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TX2_SCREEN_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RX2_SCREEN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(RX2_SCREEN_GPIO_Port, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Channel7;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */
  //__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);//��������2�����ж�---�п��������

  /* USER CODE END USART2_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = TX3_The_Cloud_Site_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TX3_The_Cloud_Site_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = RX3_The_Cloud_Site_D_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(RX3_The_Cloud_Site_D_GPIO_Port, &GPIO_InitStruct);

    /* USART3 DMA Init */
    /* USART3_RX Init */
    hdma_usart3_rx.Instance = DMA1_Channel3;
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode = DMA_NORMAL;
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart3_rx);

    /* USART3_TX Init */
    hdma_usart3_tx.Instance = DMA1_Channel2;
    hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_tx.Init.Mode = DMA_NORMAL;
    hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart3_tx);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();
  
    /**UART4 GPIO Configuration    
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX 
    */
    HAL_GPIO_DeInit(GPIOC, TX4_TO_485_Pin|RX4_TO_485_Pin);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, TX1_TO_1310_Pin|RX1_TO_1310_Pin);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, TX2_SCREEN_Pin|RX2_SCREEN_Pin);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOB, TX3_The_Cloud_Site_Pin|RX3_The_Cloud_Site_D_Pin);

    /* USART3 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
/*************************************************
  * ��������: �ض���c�⺯��printf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fputc(int ch,FILE *f)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10);//ԭ��ʹ������ʽ����
  return ch;
}
/**
  * ��������: �ض���c�⺯��getchar,scanf��DEBUG_USARTx
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
int fgetc(FILE * f)
{
  uint8_t ch = 0;
  while(HAL_UART_Receive(&huart2,&ch, 1, 0xffff)!=HAL_OK);
  return ch;
}
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    //HAL_UART_DMAStop(&huart2); ����                                                    //ֹͣ����DMA����
    uint8_t data_length  = UART2_BUFF_MAX - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);   /*������յ������ݳ���*/
    //printf("Receive Data(length = %d): ",data_length);                            //�ض���δ��������
    //HAL_UART_Transmit(&huart2, (uint8_t *)screen_tx_buff, data_length, 0xffff);
    //HAL_UART_Transmit_DMA(&huart2, (uint8_t *)screen_tx_buff,strlen(screen_tx_buff));//����ʹ��--���Խ������ݣ�����ת��
    //HAL_UART_Transmit(&huart2,(uint8_t *)screen_rx_buff,data_length,0x200);     //���Ժ����������յ������ݴ�ӡ��ȥ
    //printf("\r\n");                                                                //�ض���δ��������
    //HAL_UART_Transmit(&huart2,"\r\n", 1, 0xffff);                                 /*--ʹ��DMA���Դ˺���δ��*/
    if(data_length <=0 || data_length > UART2_BUFF_MAX) 
    {
        //__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);   /*�򿪿����ж�---*/
        HAL_UART_Receive_DMA(&huart2, (uint8_t*)UART2_BUFF, UART2_BUFF_MAX);
        return ;
    }
    DQ_putData(&screen_circ, (uint8_t*)&data_length, 1);/*�Ƚ����ݳ��ȼ�¼��������*/
    DQ_putData(&screen_circ, (uint8_t*)UART2_BUFF, (uint32_t) data_length);/*�����ݼ�¼��������*/
    //CQ_putData(&screen_circ, (uint8_t*)UART2_BUFF, (uint32_t) data_length);/*�����ݼ�¼��������*/
    memset(UART2_BUFF,0,data_length);
    data_length = 0;
    //__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);   /*�򿪿����ж�---*/
    //__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE); /*�򿪽����ж�*/
    HAL_UART_Receive_DMA(&huart2, (uint8_t*)UART2_BUFF, UART2_BUFF_MAX);/*��������2DMA����       ������ʼDMA���� ÿ��128�ֽ�����*/
}

void USER_UART2_IRQHandler(UART_HandleTypeDef *huart)
{
    if(USART2 == huart2.Instance)                                   /*�ж��Ƿ��Ǵ���2*/
    {
        screen_SLEEP_MODE = 0;
        /*if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE)!=RESET))//�ж��Ƿ��ǽ����ж�
        {
            if(uart2RxState==UART_RX_STATE_READY)   //���յ�һ֡�еĵ�һ���ֽ�
            {
                  uart2RxState=UART_RX_STATE_START;
				  __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);   //�򿪿����ж�
				  uart2RxCounter=0;                             //��������
				  screen_rx_buff[uart2RxCounter]=(uint8_t)(huart2.Instance->DR & (uint8_t)0x00FF);
				  uart2RxCounter++;
              
            }
            else if(uart2RxState==UART_RX_STATE_START)  
            {
                  screen_rx_buff[uart2RxCounter]=(uint8_t)(huart2.Instance->DR & (uint8_t)0x00FF);
                  uart2RxCounter++;
            }
            __HAL_UART_CLEAR_FLAG(&huart2,UART_FLAG_RXNE);
        
        }*/
        if((__HAL_UART_GET_FLAG(&huart2 , UART_FLAG_IDLE) != RESET))  // �ж��Ƿ��ǿ����ж�---
        //if((__HAL_UART_GET_FLAG(&huart2 , UART_FLAG_RXNE) != RESET)) 
        {
            HAL_UART_DMAStop(&huart2);
            __HAL_UART_CLEAR_IDLEFLAG(&huart2);                     /*��������жϱ�־�������һֱ���Ͻ����жϣ�*/
            //__HAL_UART_DISABLE_IT(&huart2,UART_IT_IDLE);    /*���˿����ж�---�Ϸ�������ÿ��б�־������������++*/
            //__HAL_UART_CLEAR_FLAG(&huart2,UART_FLAG_RXNE);//---
            //__HAL_UART_DISABLE_IT(&huart2,UART_IT_RXNE);    //���˽�������ж�RXNE�ж�һ��ֻ�ܽ���һ���ֽ�++
            //printf("\r\nUART1 Idle IQR Detected\r\n"); //�ض���δ��������
            //HAL_UART_Transmit(&huart2, "\r\nUART1 Idle IQR Detected\r\n", 1, 0xffff);
            USAR_UART_IDLECallback(huart);                          /*�����жϴ�������*/
        }
    }
}

void USER_UART1_IRQHandler(UART_HandleTypeDef *huart)
{
  if(USART1 == huart1.Instance)/*cc1310���ݴ���*/
      {
          if((__HAL_UART_GET_FLAG(&huart1 , UART_FLAG_IDLE) != RESET))
          {
              __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
              HAL_UART_DMAStop(&huart1);
              uint8_t data_length  = UART1_BUFF_MAX - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
              CQ_putData(&cc1310_circ, (uint8_t*)UART1_BUFF, (uint32_t) data_length);//�����ݼ�¼��������
              memset(UART1_BUFF,0,data_length);
              data_length = 0;
              __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);   //�򿪿����ж�---
              HAL_UART_Receive_DMA(&huart1, (uint8_t*)UART1_BUFF, UART1_BUFF_MAX);//��������2DMA����       //������ʼDMA���� ÿ��64�ֽ�����
          }
      }

}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/