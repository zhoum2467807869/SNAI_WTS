/*屏幕通讯接口*/

#include "screen_com.h"
#include "rtc.h"

/*
*接收页面信息：66 03 FF FF FF  页面id 03
*接收控件信息：0X70+字符串内码+结束符
*主机接收解析：FF EE 08(len) 01(跳转页面) 高位值  低位值 CRL16_L CRC16_H
*
*cmd:01跳转页面并刷新 02刷新当前页面 03返回上级页面并持续刷新 04更新参数数据  other默认自动匹配并刷新页面
*页面名称组格式:p[页面ID].b[控件].属性
*p[2].b[4].txt="456"             ID为2的页面中，ID为4的控件的txt属性赋值为"456"
*p[n0.val].b[n1.val].txt="456"   ID为n0.val的页面中，ID为n1.val的控件的txt属性赋值为"456"
*/
char  screen_tx_buff[SCREEN_TX_BUF_MAX] = {0};
char  screen_rx_buff[SCREEN_RX_BUF_MAX] = {0};
uint8_t screen_EOF_TxBuf[3] = {0xFF,0xFF,0xFF};//带FE crc16效验，未使用
uint8_t CMD_SCREEN[10]={0};/*每次从环形缓存区取出-->解析数据包存储区*/
unsigned short screen_crc_ret = 0;
unsigned char screen_crc_L,screen_crc_H;/*CRC校验存储*/
screen_other_data_t miscellany;//其他数据：页面，目标id，数据
CQ_handleTypeDef screen_circ;//环形区
//"rest","page ","get ","sendme"
volatile uint8_t screen_pageid_CURRENT = 0;/*实时获取的页面ID*/
volatile uint8_t screen_pageid_OLD = 0;/*实时获取的页面ID的上一级页面ID*/
volatile uint8_t screen_SLEEP_MODE = 0;/*睡眠模式标志位*/
/*包解析*/
int screen_main_opt(void)
{

    uint8_t LEN[1]={0};
    if(screen_SLEEP_MODE == 0)
    {
        CQ_getData(&screen_circ, LEN, 1);/*得到总包的长度*/
        while((LEN[0] >= 10 || LEN[0] == 0)){
          CQ_getData(&screen_circ, LEN, 1);/*跳过空包*/
          osDelay(100);
        }
        CQ_getData(&screen_circ, CMD_SCREEN, (uint32_t)LEN[0]);
        if((CMD_SCREEN[0] != 0xFF && CMD_SCREEN[1] != 0xEE && CMD_SCREEN[2] <= 0x00) \
            &&(CMD_SCREEN[0] != 0x66)\
            && CMD_SCREEN[0] != 0x86 \
            && CMD_SCREEN[0] != 0x1A)/*判断头部和长度是否正确*/
        {
            return -1;
        }
        if(CMD_SCREEN[2] == 8)
        {
          screen_crc_ret = CRC_Return(CMD_SCREEN,CMD_SCREEN[2]-2);
          screen_crc_L = (unsigned char)(screen_crc_ret &0x00FF);
          screen_crc_H = (unsigned char)((screen_crc_ret>>8)&0x00FF);
          if(screen_crc_L != CMD_SCREEN[6] && screen_crc_H != CMD_SCREEN[7])/*校验CRC*/
          {
            return -1;
          }   
        }
        if(CMD_SCREEN[0] == 0x86)/*睡眠模式标志*/
        {
          screen_SLEEP_MODE = 1;
          return -1;
        }
        if(CMD_SCREEN[0] == 0x66 || CMD_SCREEN[0] == 0x1A)/*返回屏幕信息，或者参数有误说明页面不对需要刷新页面号*/
        {
          goto refresh_pageid;
        
        }
        switch (CMD_SCREEN[3])/*判断命令类型*/
        {
            case 0x01:
              screen_goto_page(CMD_SCREEN[5]);/*跳转指定页面*/
              break;
            case 0x02:
              
              break; 
            case 0x03:
                screen_goto_page(screen_pageid_OLD);/*报警手动返回上级页面*/
              break;
            case 0x04: 
                CQ_getData(&screen_circ, LEN, 1);/*得到长度为10的包，其第二部分长度,--空闲中断模式下10长的包需要两次收到，故解析两次*/
                if(LEN[0] >= 1 && LEN[0] <= 10)
                {
                  CQ_getData(&screen_circ, CMD_SCREEN+6, (uint32_t)LEN[0]);/*取出*/
                  screen_get_par(CMD_SCREEN[4],CMD_SCREEN[5],CMD_SCREEN+6);/*获取设置参数*/
                }
              break;    
            default: 
              break;
        }
        memset(CMD_SCREEN,0,10);
    }
    return 0;
refresh_pageid:
    if(CMD_SCREEN[2] == 0xFF && CMD_SCREEN[3] == 0xFF && CMD_SCREEN[4] == 0xFF && CMD_SCREEN[1]<=12)/*页码范围内*/
    {
      if(screen_pageid_CURRENT != CMD_SCREEN[1])/*实际和目前页面ID不同则--更新页面*/
      {
        screen_pageid_OLD = screen_pageid_CURRENT;
        screen_pageid_CURRENT = CMD_SCREEN[1];//跟新新页面id
 
      }
      
    }
    return 0;
}


/*
*跳转指定页面 --报警触发用
*参数页面id
*/
int screen_goto_page(uint8_t pageid)
{
    char cmd_buf[10] = {0};
    sprintf(cmd_buf,"page %d",pageid);
    //HAL_UART_Transmit_DMA(&huart2, (uint8_t*)cmd_buf, strlen(cmd_buf));
    HAL_UART_Transmit(&huart2,(uint8_t*)cmd_buf,strlen(cmd_buf),0x400);//优先阻塞式选择带超时，保证一定时间内发送出去
    HAL_UART_Transmit(&huart2,(uint8_t*)screen_EOF_TxBuf,strlen((char const*)screen_EOF_TxBuf),0x400);
    //osDelay(2);
   return 0; 
}

/*
*主动获得页面id
*
*/
void screen_pull_page(void)
{
    char cmd_buf[10] = {0};
    sprintf(cmd_buf,"sendme");
    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)cmd_buf, strlen(cmd_buf));
    osDelay(2);
    HAL_UART_Transmit_DMA(&huart2,(uint8_t*)screen_EOF_TxBuf,strlen((char const*)screen_EOF_TxBuf));
    //HAL_UART_Transmit(&huart2,(uint8_t*)cmd_buf,strlen(cmd_buf),0x400);//优先阻塞式选择带超时，保证一定时间内发送出去
    //HAL_UART_Transmit(&huart2,(uint8_t*)screen_EOF_TxBuf,strlen((char const*)screen_EOF_TxBuf),0x400);
}


/*获取设置的参数写入
*
*参数：页面id号  控件序号  参数
*
*/
int screen_get_par(uint8_t pageid,uint8_t par_id ,void *data)
{

/* "main",  0
  "menu",   1
  "node1",  2
  "node2",  3
  "node3",  4
  "node4",  5
  "almset", 6
  "property"7
  "alarm",  8
  "clock"   9  */
  uint16_t temp_data = atoi((char*)data);
  float temp_float = atof((char*)data);
  switch(pageid)
  {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      switch(par_id)
      {
        case 0x01:/*信道*/
          sprintf(screen_tx_buff,"set channel par:%d\n",temp_data);
          //HAL_UART_Transmit_DMA(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff));/*调试用*/
          osDelay(2);
          CC1310_Main_Com(temp_data);
          break;
        case 0x02:/*节点地址*/
          sprintf(screen_tx_buff,"node addr par:%d\n",temp_data);
          //HAL_UART_Transmit_DMA(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff));/*调试用*/
          osDelay(2);
          
          break;
        case 0x03:/*扫描周期*/
          sprintf(screen_tx_buff,"scanf cycle par:%d\n",temp_data);
          //HAL_UART_Transmit_DMA(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff));/*调试用*/
          osDelay(2);
          
        break;
        case 0x04:/*温度报警上限*/
          temp_data = *(uint8_t *)data;
          sprintf(screen_tx_buff,"temp upper limit par:%.1f\n",temp_float);
          //HAL_UART_Transmit_DMA(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff));/*调试用*/
          osDelay(2);
          
          break;
        case 0x05:/*温度报警下限*/
          sprintf(screen_tx_buff,"temp lower limit par:%.1f\n",temp_float);
          //HAL_UART_Transmit_DMA(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff));/*调试用*/
          osDelay(2);
          
          break;
        case 0x06:/*允许超时min*/
          sprintf(screen_tx_buff,"allowed the timeout par:%d min\n",temp_data);
          if(data!= 0)
          //HAL_UART_Transmit_DMA(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff));/*调试用*/
          osDelay(2);
          
          break;
        case 0x07:/*允许超时次数*/
          sprintf(screen_tx_buff,"allowed the timeout num par:%d\n",temp_data);
          //HAL_UART_Transmit_DMA(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff));/*调试用*/
          osDelay(20);
          HAL_UART_Transmit(&huart2,"p604.pic=68",strlen("p604.pic=68"),0x400);
          HAL_UART_Transmit(&huart2,(uint8_t*)screen_EOF_TxBuf,3,0x400);
          //HAL_UART_Transmit(&huart2, (uint8_t*)state, 14,0x400);/*调试用*/
          //osDelay(200);
          //HAL_UART_Transmit_DMA(&huart2,(uint8_t*)screen_EOF_TxBuf,3);
          break;
      }
      break;
    case 7:
      
      break;
    case 8:
      
      break;
    case 9:
      switch(par_id)
      {
        case 0x01:/*年*/
          sprintf(screen_tx_buff,"set year:%d\n",temp_data);
          //HAL_UART_Transmit(&huart2,(uint8_t*)screen_tx_buff,strlen((char const*)screen_tx_buff),0x400);/*调试用*/
          current_date_time.year = temp_data;
          break;
        case 0x02:/*月*/
          sprintf(screen_tx_buff,"set month:%d\n",temp_data);
          //HAL_UART_Transmit(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff),0x400);/*调试用*/
          current_date_time.month = temp_data;
          break;
        case 0x03:/*日*/
          sprintf(screen_tx_buff,"set day:%d\n",temp_data);
          //HAL_UART_Transmit(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff),0x400);/*调试用*/
          current_date_time.day = temp_data;
        break;
        case 0x04:/*时*/
          sprintf(screen_tx_buff,"set hour:%d\n",temp_data);
          //HAL_UART_Transmit(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff),0x400);/*调试用*/
          current_date_time.hour = temp_data;
          break;
        case 0x05:/*分*/
          sprintf(screen_tx_buff,"set min:%d\n",temp_data);
          //HAL_UART_Transmit(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff),0x400);/*调试用*/
          current_date_time.minute = temp_data;
          RTC_Set_Time(current_date_time.hour,current_date_time.minute,0);	        /*设置时间 ,根据实际时间修改*/
          RTC_Set_Date(0,current_date_time.month,current_date_time.day,current_date_time.year-2000);	/*设置日期*/
          
      
          HAL_UART_Transmit(&huart2,"p902.pic=68",strlen("p902.pic=68"),0x400);
          HAL_UART_Transmit(&huart2,(uint8_t*)screen_EOF_TxBuf,3,0x400);
          break;
      default:
        break;
      }
    break;
    default:
      break;
  }
	return 0;
}

/*更新当前屏幕参数数值--获取页面--获取指定参数*/
int screen_get_current_val(int id)
{
  switch (id)
  {
    case 1:
      
        osDelay(2);

      break;
    default:
      break;
  }
	return 0;
}

/*更新当前屏幕参数数值----循环更新参数*/
int screen_update_val(void *data)
{
    
    uint8_t pageid = *(uint8_t *)data;
    switch (pageid)
    {
    case 0:
      
      break;
      
    case 1:
      
      break;
      
    default:
      break;
    
    }
	return 0;
}

int screen_alarm_update(void *data)
{
  
  return 0;
}

void Get_MCUUID(void)
{
	uint32_t UID[3];
	UID[0]=HAL_GetUIDw0();   /*读取芯片的96位唯一标识ID*/
    UID[1]=HAL_GetUIDw1(); 
    UID[2]=HAL_GetUIDw2(); 
	sprintf((char*) screen_tx_buff + 24, "%0*lx", 8, UID[0]);
	sprintf((char*) screen_tx_buff + 32, "%0*lx", 8, UID[1]);
	sprintf((char*) screen_tx_buff + 40, "%0*lx", 8, UID[2]);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t *)screen_tx_buff+24, strlen(screen_tx_buff+24));
    osDelay(10);
    sprintf(screen_tx_buff,"qr700.txt=\"%s\"",screen_tx_buff + 24);
    HAL_UART_Transmit(&huart2,(uint8_t*)screen_tx_buff,strlen(screen_tx_buff),0x400);//优先阻塞式选择带超时，保证一定时间内发送出去
    HAL_UART_Transmit(&huart2,(uint8_t*)screen_EOF_TxBuf,strlen((char const*)screen_EOF_TxBuf),0x400);
}

void floatToString(float data, uint8_t zbit, uint8_t xbit, uint8_t *str)
{
	 int i=0, j=0, k;

	 const float code[]={0.5, 0.050, 0.0050, 0.00050, 0.000050};

	 uint32_t temp, tempoten;
	 uint8_t intpart[10], dotpart[10];  // 数的整数部分和小数部分

	 memset(str, '\0', zbit + xbit);
	 /*
	  * 如果是0 则直接处理
	  */
	 if ((data >= -0.00001) && (data <= 0.00001)) {
	  str[0] = '0';
	  return;
	 }

	 //1.确定符号位
	 if (data < 0) {
	  str[0] = '-';
	  data = -data;
	 } else
	  str[0] = '+';

	 //2.确定整数部分
	 if(xbit>5){
	  xbit = 5;
	 }
	 data += code[xbit];

	 temp = (uint32_t) data;
	 i = 0;
	 tempoten = temp / 10;
	 while (tempoten != 0) {
	  intpart[i] = temp - 10 * tempoten + 48; //to ascii code
	  temp = tempoten;
	  tempoten = temp / 10;
	  i++;
	 }
	 intpart[i] = temp + 48;

	 //3.确定小数部分


	 data = data - (uint32_t)data;

	// data = roundingNum(data);

	 for (j = 0; j < xbit; j++) {
	  dotpart[j] = (int) (data * 10) + 48;
	  data = data * 10.000000;
	  data = data - (uint32_t) data;
	 }

	 for (k = 1; k <= i + 1; k++)
	  str[k] = intpart[i + 1 - k];
	 str[i + 2] = '.';
	 for (k = i + 3; k < i + j + 3; k++)
	  str[k] = dotpart[k - i - 3];

	 if(str[0] == '+'){
	  for (uint32_t k = 0; k < j+i+2; k++) {
	   str[k] = str[k+1];
	  }
	  str[j+i+2] = 0;
	 }

}
