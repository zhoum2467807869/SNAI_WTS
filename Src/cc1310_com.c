/*
 *  FILE: cc1310_com.c
 *
 *  Created on: 2020/2/24
 *
 *         Author: aron66
 *
 *  DESCRIPTION:--
 */
#ifdef __cplusplus //use C compiler
extern "C" {
#endif
#include "cc1310_com.h"
uint8_t CC1310_RX_BUF[CC1310_RX_BUF_MAX] = {0};
uint8_t CC1310_TX_BUF[CC1310_TX_BUF_MAX] = {0xFF,DEVICE_TYPE,0x06,0x00,0x00,0x00};
SensorTab_t SensorTab[SENSOR_NUM];//传感器最大支持50个传感器数据存储，以信号质量分0-12高质量区域，13-24中等信号区域，剩余弱信号区域
static uint8_t device_count = 0;

/******************************************************************************
*协议格式：FF 02 LEN CHINNEL SIGNAL ADDR VAL_H VAL_L CRC16_H CRC16_L  主机接收 高低位？？？
*
*协议格式：FF 01 LEN SET_CHINNEL CRC16_H CRC16_L 主机发送，设置信道
*******************************************************************************/

//超时上报，报警（次数，时间）
CQ_handleTypeDef cc1310_circ;//环形区
void CC1310_Opt_init(void)
{
    for(uint8_t i=0;i<50;i++)
    {
        SensorTab[i].addr_num = 0;
    }
}
int CC1310_Opt_Loop(void)
{
    unsigned short CRC_Ret = 0;
    unsigned char cc1310_crc_L,cc1310_crc_H;
    uint8_t update_flag = 0;
    uint8_t i = 0;
    uint8_t CC1310_CMD[10] = {0};
    uint8_t LEN[1]={0};
    CQ_getData(&cc1310_circ, LEN, 1);/*得到总包的长度*/
    CQ_getData(&cc1310_circ, CC1310_CMD, (uint32_t)LEN[0]);
    if((CC1310_CMD[0] != 0xFF && CC1310_CMD[1] != 0x02 && CC1310_CMD[2] <= 0x00))/*判断头部和长度是否正确*/
    {
        return -1;
    }
    if(CC1310_CMD[2] == 0x0A)
    {
      CRC_Ret = CRC_Return(CC1310_CMD,CC1310_CMD[2]-2);
      cc1310_crc_L = (unsigned char)(CRC_Ret &0x00FF);
      cc1310_crc_H = (unsigned char)((CRC_Ret>>8)&0x00FF);
      if(cc1310_crc_L != CC1310_CMD[9] && cc1310_crc_H != CC1310_CMD[8])/*校验CRC*/
      {
        return -1;
      }
      
      for(;i <= device_count;i++)
      {
        if(SensorTab[i].addr_num != CC1310_CMD[5])
        {
          continue; /*到达尾部*/ 
        }
        else /*库里存在*/
        {
          SensorTab[i].value = 0;
          SensorTab[i].offline_count = 0;
          SensorTab[i].current_min = 0;
          SensorTab[i].channel = 0;
          SensorTab[i].signal = CC1310_CMD[4];
          update_flag = 1;
          break;
        }
      }
      SensorTab[i].addr_num = 0;
      SensorTab[i].channel = 0;
      SensorTab[i].current_min = 0;
      SensorTab[i].offline_count = 0;
      SensorTab[i].signal = CC1310_CMD[4];
      SensorTab[i].value = 0;
      if(update_flag != 1)
      {
          if(device_count < SENSOR_NUM)
          {
              device_count++;
          }
      }
      update_flag = 0;
    /*强信号，存储1-12号*/
    /*中等信号强度，存储13-24号*/
    /*弱信号，存储其他区*/
    }
    return 0;
}


int CC1310_Main_Com(uint8_t data)
{
    unsigned short CRC_Ret = 0;
    unsigned char cc1310_crc_L,cc1310_crc_H;
    CC1310_TX_BUF[3] = data;
    CRC_Ret = CRC_Return((unsigned char *)CC1310_TX_BUF, 4);
    cc1310_crc_L = (unsigned char)(CRC_Ret &0x00FF);
    cc1310_crc_H = (unsigned char)((CRC_Ret>>8)&0x00FF);
    CC1310_TX_BUF[4] = cc1310_crc_H;
    CC1310_TX_BUF[5] = cc1310_crc_L;
    HAL_UART_Transmit_DMA(&huart1,(uint8_t*)CC1310_TX_BUF,6);
    return 0;
}    
#ifdef __cplusplus //end extern c
}
#endif
