/*
 * CircularQueue.h
 *
 *  Created on: 2019Äê8ÔÂ21ÈÕ
 *      Author: Breuning
 */

#ifndef CIRCULARQUEUE_H_
#define CIRCULARQUEUE_H_

#include "main.h"

#define GET_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define IS_POWER_OF_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))

typedef struct
{
	uint8_t *dataBufer;
	uint32_t size;
	uint32_t entrance;
	uint32_t exit;
    uint8_t cmd_len;
}CQ_handleTypeDef;

BOOL  CQ_init(CQ_handleTypeDef *CircularQueue ,uint8_t *memAdd,uint16_t len);
BOOL  CQ_isEmpty(CQ_handleTypeDef *CircularQueue);
BOOL  CQ_isFull(CQ_handleTypeDef *CircularQueue);
void  CQ_emptyData(CQ_handleTypeDef*CircularQueue);
uint32_t CQ_getLength(CQ_handleTypeDef *CircularQueue);
uint32_t CQ_getData(CQ_handleTypeDef *CircularQueue, uint8_t *targetBuf, uint32_t len);
uint32_t CQ_putData(CQ_handleTypeDef *CircularQueue, uint8_t *sourceBuf, uint32_t len);
uint32_t DQ_putData(CQ_handleTypeDef *CircularQueue, uint8_t *sourceBuf, uint32_t len);
#endif /* CIRCULARQUEUE_H_ */
