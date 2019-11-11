/*
 * screen_com.h
 *
 *  Created on: Oct 17, 2019
 *      Author: 47282
 */

#ifndef SCREEN_COM_H_
#define SCREEN_COM_H_
#include "stm32f1xx_hal.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "CircularQueue.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cc1310_com.h"
#include "main.h"
//#pragma pack(8)
#define SCREEN_TX_BUF_MAX 64
#define SCREEN_RX_BUF_MAX 256
#define SCREEN_COUNT 10
#define MAIN_SCREEN 0U
#define MENU_SCREEN 1U
#define NODE1_SCREEN 2U
#define NODE2_SCREEN 3U
#define NODE3_SCREEN 4U
#define NODE4_SCREEN 5U
#define ALMSET_SCREEN 6U
#define PROPERTY_SCREEN 7U
#define ALARM_SCREEN 8U
#define CLOCK_SCREEN 9U

typedef struct screen_data_other
{
	int page_id;
	char char_val[64];
	int tar_id;
}screen_other_data_t;

extern volatile uint8_t screen_pageid_CURRENT,screen_pageid_OLD;
extern volatile uint8_t screen_SLEEP_MODE;
extern char  screen_tx_buff[];
extern char  screen_rx_buff[];
extern uint8_t screen_EOF_TxBuf[];
extern CQ_handleTypeDef screen_circ;

int screen_main_opt(void);
int screen_update_val(void *data);
int screen_get_current_val(int id);
void floatToString(float data, uint8_t zbit, uint8_t xbit, uint8_t *str);
int screen_alarm_update(void *data);
int screen_get_par(uint8_t num,uint8_t par_id,void *data);
int screen_goto_page(uint8_t pageid);
void screen_pull_page(void);
void get_resert_status(void);
void Get_MCUUID(void);
#endif /* SCREEN_COM_H_ */
