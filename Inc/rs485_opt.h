/*
 * RS485_OPT_H
 *
 *  Created on: Oct 19, 2019
 *      Author: 47282
 */
#ifndef RS485_OPT_H_
#define RS485_OPT_H_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define RS485_TX_BUF_MAX 128
#define RS485_RX_BUF_MAX 128

extern uint8_t RS485_TX_BUF[RS485_TX_BUF_MAX];
extern uint8_t RS485_TX_BUF[RS485_TX_BUF_MAX];
int Post_485_Opt(int * data);




























#endif /*RS485_OPT_H_*/