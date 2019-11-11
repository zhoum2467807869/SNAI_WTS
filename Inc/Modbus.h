/*
 * Modbus.h
 *
 *  Created on: 2019年8月17日
 *      Author: Breuning
 */

#ifndef MODBUS_H_
#define MODBUS_H_

#include "main.h"

typedef struct
{
	#define RS485_RX_LEN  128
	#define RS485_TX_LEN  128


	#define RS485_RX_WRITE_COUNT  (7+24*2+2)   	//  7个字节:    从机地址+功能码+寄存器起始地址高位+寄存器起始地址低位+寄存器数量高位+寄存器数量低位+字节数
	                                     	 	//  24*2个字节: 23为存放数据的寄存器的数量，每个寄存器分高位与低位2个字节
	                                     	 	//  2个字节:    CRC校验高位+CRC校验低位
	#define RS485_RX_READ_COUNT   (6+2)			//  6个字节：     从机地址+功能码+寄存器起始地址高位+寄存器起始地址低位+寄存器数量高位+寄存器数量低位
												//  2个字节:    CRC校验高位+CRC校验低位

	#define RS485_RX_IMCOMPLETE         0
	#define RS485_RX_WRITE_COMPLETE     1
    #define RS485_RX_READ_COMPLETE		2

//	#define RS485_RX_PROCESS_BUSY 1
//	#define RS485_RX_PROCESS_IDLE 0
	uint8_t RX_ProcessSta;
	uint8_t RX_State;
	uint8_t RX_Len;
}RS485_UART;

typedef struct
{
	#define  DATA_LEN  8
	uint8_t  RETURN_VALUE;
    uint8_t  RX_DATA[RS485_RX_LEN];
    uint8_t  TX_DATA[RS485_TX_LEN];
    uint8_t  ERR_CODE;
	uint8_t  DEV_ADDR;
    uint8_t  FUN_CODE;
	uint8_t  TIMEOUT_CNT;
	uint16_t CRC_VAL;
    uint16_t CHECK_CRC;
    uint16_t REG_ADDR;
    uint16_t REG_QUAN;
}MODBUS;

typedef enum
{
	FUN_ERR     = 0x01,
	ADDR_ERR    = 0x02,
	VALUE_ERR   = 0x03,
	TIMEOUT_ERR	= 0x10,
	DATA_ERR    = 0x11,
	CRC_ERR     = 0x18,
}ERR_TYPE;

typedef enum
{
	SLAVE_DEV_ADDR_NUM = 0,
	FUNC_NUM           = 1,
	REG_ADDR_HIGH_NUM  = 2,
	REG_ADDR_LOW_NUM   = 3,
	REG_NUM_HIGH_NUM   = 4,
	REG_NUM_LOW_NUM    = 5,

	RX_EFCT_DATA_OFFSET_NUM = 7        //从第8个字节开始为寄存器值，即传过来的真正有用的数据值
}RecvDataPacket_Num;

typedef enum
{
	SLAVE_DEV_ADDR_VALUE = 21,
	READ_FUN_VALUE       = 3,
	WRITE_FUN_VALUE      = 16,
	READ_REG_QUAN_VALUE   = 17
}MATCH_VALUE;

typedef enum
{
	RECEIVE = 0,
    TRANSMIT = 1
}RS485_STATE;

void MODBUS_Save485Data(void);
void ModbusDataRecv(void);
void ModbusDataAnalysis(uint8_t *modbusbuf,uint8_t bufLen);
void ModBus_MCUID_Ack(uint8_t *modbusbuf,uint8_t bufLen);
void WriteMultiRegister(uint8_t *modbusbuf);
void RS485Uart_RX_TX_Switch(RS485_STATE RS485state);
unsigned int CRC16(unsigned char *puchMsg, unsigned int usDataLen);


extern uint8_t ModbusBuf[RS485_RX_LEN];
extern uint8_t RS485_RX[];
extern RS485_UART RS485Uart;




#endif /* MODBUS_H_ */
