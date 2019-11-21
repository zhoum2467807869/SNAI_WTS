#include "W25Qxx.h"

static void HAL_W25QXX_CS_ENABLE(void);
static void HAL_W25QXX_CS_DISABLE(void);
static void HAL_SPI_Send_Byte(uint8_t byte);
static uint8_t HAL_SPI_Receive_Byte(void);

void HAL_W25QXX_Wait_Busy(void);
/* USER CODE BEGIN 0 */
uint16_t W25QXX_TYPE=W25Q128;	//默认是W25Q128
/* USER CODE END 0 */

static void HAL_W25QXX_CS_ENABLE(void)
{
	HAL_GPIO_WritePin(W25Q64_CS_GPIO_Port,W25Q64_CS_Pin, GPIO_PIN_RESET);
}

static void HAL_W25QXX_CS_DISABLE(void)
{
	HAL_GPIO_WritePin(W25Q64_CS_GPIO_Port,W25Q64_CS_Pin, GPIO_PIN_SET);
}

static void HAL_SPI_Send_Byte(uint8_t byte)
{
	HAL_SPI_Transmit(&hspi2,&byte,1,0xFFFF);
}
static uint8_t HAL_SPI_Receive_Byte(void)
{
	uint8_t data = 0xFF;
	HAL_SPI_Receive(&hspi2,&data,1,0xFFFF);
	return data;
}
//读取芯片ID
//返回值如下:				   
//0XEF13,代表芯片型号为W25Q80  
//0XEF14,代表芯片型号为W25Q16    
//0XEF15,代表芯片型号为W25Q32  
//0XEF16,代表芯片型号为W25Q64 
//0XEF17,代表芯片型号为W25Q128 	  
//0XEF18,代表芯片型号为W25Q256
uint32_t Temp = 0;
uint32_t HAL_W25QXX_ReadID(void)
{
  /*选择串行FLASH: CS低电平 */
  HAL_W25QXX_CS_ENABLE();
  /* 发送命令：读取 芯片ID */
  HAL_SPI_Send_Byte(W25X_ManufactDeviceID);
	HAL_SPI_Send_Byte(0x00);
	HAL_SPI_Send_Byte(0x00);
	HAL_SPI_Send_Byte(0x00);
  Temp |= HAL_SPI_Receive_Byte()<<8;
  Temp |= HAL_SPI_Receive_Byte();
  HAL_W25QXX_CS_DISABLE();
  return Temp;
}
//读取W25QXX的状态寄存器，w25qXX一共有3个状态寄存器
//状态寄存器1
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0，状态寄存器保护位，配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
//状态寄存器2
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//状态寄存器3
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:状态寄存器号，范围 :1~3
//返回值:状态寄存器值
uint8_t HAL_W25QXX_Read_SR(uint8_t regno)
{
	uint8_t byte=0,command=0; 
	switch(regno)
	{
		case 1:
			command=W25X_ReadStatusReg1;    //读状态寄存器1指令
			break;
		case 2:
			command=W25X_ReadStatusReg2;    //读状态寄存器2指令
			break;
		case 3:
			command=W25X_ReadStatusReg3;    //读状态寄存器3指令
			break;
		default:
			command=W25X_ReadStatusReg1;    
			break;
	}    
	HAL_W25QXX_CS_ENABLE();                            //使能器件   
	HAL_SPI_Send_Byte(command);            //发送读取状态寄存器命令   
	byte=HAL_SPI_Receive_Byte();          //发送一个字节
	HAL_W25QXX_CS_DISABLE();                            //取消片选    
	return byte;   
}

//写W25QXX状态寄存器
void HAL_W25QXX_Write_SR(uint8_t regno,uint8_t sr)   
{   
	uint8_t command=0;
	switch(regno)
	{
		case 1:
			command=W25X_WriteStatusReg1;    //写状态寄存器1指令
			break;
		case 2:
			command=W25X_WriteStatusReg2;    //写状态寄存器2指令
			break;
		case 3:
			command=W25X_WriteStatusReg3;    //写状态寄存器3指令
			break;
		default:
			command=W25X_WriteStatusReg1;    
			break;
	}   
	HAL_W25QXX_CS_ENABLE();                            //使能器件   
	HAL_SPI_Send_Byte(command);            //发送写取状态寄存器命令   
	HAL_SPI_Send_Byte(sr);                 //写入一个字节 
	HAL_W25QXX_CS_DISABLE();                            //取消片选     	      
} 
//等待空闲
void HAL_W25QXX_Wait_Busy(void)   
{   
	while((HAL_W25QXX_Read_SR(1)&0x01)==0x01);   //等待BUSY位清空
}
//W25QXX写使能
//将WEL置位 
void HAL_W25QXX_Write_Enable(void)   
{
	HAL_W25QXX_CS_ENABLE();                            //使能器件
    HAL_SPI_Send_Byte(W25X_WriteEnable);  				//发送写使能 
	HAL_W25QXX_CS_DISABLE();                           //取消片选     	      
} 
//W25QXX写使能
//将WEL置位    
void HAL_W25QXX_Write_Disable(void)   
{
	HAL_W25QXX_CS_ENABLE();                            //使能器件   
  HAL_SPI_Send_Byte(W25X_WriteDisable);  				  //发送写使能  
	HAL_W25QXX_CS_DISABLE();                           //取消片选     	      
}
//读取SPI FLASH  
//在指定地址开始读取指定长度数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
uint8_t HAL_W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	HAL_W25QXX_CS_ENABLE();                          //使能器件 
	HAL_SPI_Send_Byte(W25X_ReadData);      //发送读取命令 
	HAL_SPI_Send_Byte((uint8_t)((ReadAddr)>>16));   //发送24bit地址    
	HAL_SPI_Send_Byte((uint8_t)((ReadAddr)>>8));   
	HAL_SPI_Send_Byte((uint8_t)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)
	{ 
		pBuffer[i]=HAL_SPI_Receive_Byte();    //循环读数 
	}
	HAL_W25QXX_CS_DISABLE();  		
	return 0;	
}  
//SPI在一页(0~65535)内写入少于256字节的数据
//在指定地址最大写入256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void HAL_W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
  HAL_W25QXX_Write_Enable();                  //SET WEL 
	HAL_W25QXX_CS_ENABLE();                            //使能器件  
	HAL_SPI_Send_Byte(W25X_PageProgram);   //发送写页命令   
	HAL_SPI_Send_Byte((uint8_t)((WriteAddr)>>16)); //发送24bit地址    
	HAL_SPI_Send_Byte((uint8_t)((WriteAddr)>>8));   
	HAL_SPI_Send_Byte((uint8_t)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++)
		HAL_SPI_Send_Byte(pBuffer[i]);//循环写数 
	HAL_W25QXX_CS_DISABLE();                            //取消片选 
	HAL_W25QXX_Wait_Busy();					   //等待写入结束
} 
//无校验写SPI FLASH 
//必须确保所写地址范围内数据都为0XFF,否则在非0XFF处写入数据将失败！
//具有自动换页功能
//在指定地址开始写入指定长度数，但确保地址不越界
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void HAL_W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)
		pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		HAL_W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)
			break;//写入结束
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	
			NumByteToWrite-=pageremain;			  //减去已经写入的字节
			if(NumByteToWrite>256)
				pageremain=256; //一次可以写入256个字节
			else 
				pageremain=NumByteToWrite; 	  //不够256个字节
		}
	}    
} 
//写SPI FLASH  
//在指定地址开始写指定长度数据
//该函数带擦除操作
//pBuffer数据存储区
//WriteAddr:开始要写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
uint8_t W25QXX_BUFFER[4096];		 
uint8_t HAL_W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t* W25QXX_BUF;	  
  W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//扇区地址 
	secoff=WriteAddr%4096;//扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小 
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		HAL_W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除	  
		}
		if(i<secremain)//需要擦除
		{
			HAL_W25QXX_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			HAL_W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  
		}
		else 
			HAL_W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的，直接写入扇区剩余空间 				   
		if(NumByteToWrite==secremain)
			break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增 1
			secoff=0;//偏移位置为 0 	 

			pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
			NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	
	return 0;	
}
//擦除整个芯片	  
//等待时间超长...
void HAL_W25QXX_Erase_Chip(void)   
{                                   
	HAL_W25QXX_Write_Enable();                  //SET WEL 
	HAL_W25QXX_Wait_Busy();   
	HAL_W25QXX_CS_ENABLE();                            //使能器件   
	HAL_SPI_Send_Byte(W25X_ChipErase);        //发送片 擦除命令
	HAL_W25QXX_CS_DISABLE();                            //取消片选     	      
	HAL_W25QXX_Wait_Busy();   				   //等待芯片擦除结束
}
//擦除一个扇区
//Dst_Addr:扇区地址，根据实际容量设置
//擦除一个扇区最少需要时间:150ms
void HAL_W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
	//监视flash擦除情况，测试用   
 	//printf("fe:%x\r\n",Dst_Addr);	  
	Dst_Addr*=4096;
	HAL_W25QXX_Write_Enable();                  //SET WEL 	 
	HAL_W25QXX_Wait_Busy();   
	HAL_W25QXX_CS_ENABLE();                            //使能器件   
	HAL_SPI_Send_Byte(W25X_SectorErase);   //发送扇区 擦除命令 
	HAL_SPI_Send_Byte((uint8_t)((Dst_Addr)>>16));  //发送24bit地址    
	HAL_SPI_Send_Byte((uint8_t)((Dst_Addr)>>8));   
	HAL_SPI_Send_Byte((uint8_t)Dst_Addr);  
	HAL_W25QXX_CS_DISABLE();                            //取消片选     	      
	HAL_W25QXX_Wait_Busy();   				    //等待芯片擦除结束
} 
//进入掉电模式
void HAL_W25QXX_PowerDown(void)   
{ 
	HAL_W25QXX_CS_ENABLE();                            //使能器件    
	HAL_SPI_Send_Byte(W25X_PowerDown);     //发送掉电命令 
	HAL_W25QXX_CS_DISABLE();                            //取消片选      	      
	HAL_Delay(1);                            //等待TPD  
}   
//唤醒
void HAL_W25QXX_WAKEUP(void)   
{  
  	HAL_W25QXX_CS_ENABLE();                                //使能器件   
    HAL_SPI_Send_Byte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB    
	HAL_W25QXX_CS_DISABLE();                               //取消片选     	      
    HAL_Delay(1);                                //等待TRES1 此处延时为1ms
}