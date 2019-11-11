/*
 * CloudRX.h
 *
 *  Created on: 2019年8月30日
 *      Author: Breuning
 */

#ifndef CLOUDRX_H_
#define CLOUDRX_H_

#include "main.h"
#include "cJSON.h"

#define RJ45_RX_LEN  128
#define cloudRxQueueLen 256
#define jsonInPutBufLen  1000

#define FUN_NUM    12
#define WINDOW_NUM  4
#define HOT_NUM     4
#define PAD_NUM     3
#define ALARM_NUM   1
#define AUTO_NUM    4

typedef struct
{
	BOOL     isRxEnable;
	BOOL     isRxJsonMsgComplete;              //是否接收到一个完整的云端下发的Json数据包
	short    RxJsonMsgCount;                   //接收到的完整的云端下发Json数据包的字节数
}cloudRxCrt_t;

extern cloudRxCrt_t cloudRxCrt;
extern uint8_t RJ45_RX[RJ45_RX_LEN];

void CLUOD_SaveRemoteData(void);
void CloudDataRecv(void);




#endif /* CLOUDRX_H_ */
