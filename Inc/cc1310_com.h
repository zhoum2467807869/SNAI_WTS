/*
 * CC1310_COM.H
 *
 *  Created on: Oct 19, 2019
 *      Author: 47282
 */
#ifndef CC1310_COM_H_
#define CC1310_COM_H_
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CC1310_TX_BUF_MAX 10
#define CC1310_RX_BUF_MAX 128
#define DEVICE_TYPE 0x01U
#define SIGNAL_CONUNT 24U
#define SENSOR_NUM 50U
/*环形缓冲区记录器*/
typedef struct cc1310_circular_buffer
{
    void *ptr;
    unsigned long count;
    unsigned long read_offset;
    unsigned long write_offset;
} cc1310_circular_buffer_t;


typedef struct SensorTab
{
	uint8_t addr_num;  //地址
	float value;    //数值
    int16_t signal;//信号强度
    uint8_t channel;//信道
    uint8_t current_min;//当前时间
    uint8_t offline_count;//离线次数
}SensorTab_t;

extern SensorTab_t SensorTab[];//传感器数据存放
extern CQ_handleTypeDef cc1310_circ;
extern uint8_t CC1310_RX_BUF[];
extern uint8_t CC1310_TX_BUF[];

extern int CC1310_Opt_Loop(void);
extern int CC1310_Main_Com(uint8_t data);
extern void CC1310_Opt_init(void);





















#endif /*CC1310_COM_H_*/