#include "rs485_opt.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint8_t RS485_RX_BUF[RS485_RX_BUF_MAX] = {0};
uint8_t RS485_TX_BUF[RS485_TX_BUF_MAX] = {0};

int Post_485_Opt(int * data)
{
    //HAL_UART_Receive_IT(&huart4, RS485_RX_BUF, 1);     	   //ʹ�ܴ���4�����жϣ����ֽڴ���
    return 0;
}

  















