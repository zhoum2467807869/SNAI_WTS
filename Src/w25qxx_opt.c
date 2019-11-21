#include "W25Qxx.h"

static void HAL_W25QXX_CS_ENABLE(void);
static void HAL_W25QXX_CS_DISABLE(void);
static void HAL_SPI_Send_Byte(uint8_t byte);
static uint8_t HAL_SPI_Receive_Byte(void);

void HAL_W25QXX_Wait_Busy(void);
/* USER CODE BEGIN 0 */
uint16_t W25QXX_TYPE=W25Q128;	//Ĭ����W25Q128
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
//?����?D???ID
//����???�̨�???:				   
//0XEF13,������?D???D��o??aW25Q80  
//0XEF14,������?D???D��o??aW25Q16    
//0XEF15,������?D???D��o??aW25Q32  
//0XEF16,������?D???D��o??aW25Q64 
//0XEF17,������?D???D��o??aW25Q128 	  
//0XEF18,������?D???D��o??aW25Q256
uint32_t Temp = 0;
uint32_t HAL_W25QXX_ReadID(void)
{
  /* ????��?DDFLASH: CS�̨���??? */
  HAL_W25QXX_CS_ENABLE();
  /* ����?��?����?��o?����?D???D��o?ID */
  HAL_SPI_Send_Byte(W25X_ManufactDeviceID);
	HAL_SPI_Send_Byte(0x00);
	HAL_SPI_Send_Byte(0x00);
	HAL_SPI_Send_Byte(0x00);
  Temp |= HAL_SPI_Receive_Byte()<<8;
  Temp |= HAL_SPI_Receive_Byte();
  HAL_W25QXX_CS_DISABLE();
  return Temp;
}
//?����?W25QXX��?���䨬???��??�¡�?W25QXX��?12��D3??���䨬???��??��
//���䨬???��??��1��o
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:??��?0,���䨬???��??�¡���?��??,??o?WP��1��?
//TB,BP2,BP1,BP0:FLASH??����D�����?�訦��??
//WEL:D�䨺1?��???��
//BUSY:?|����????(1,?|;0,???D)
//??��?:0x00
//���䨬???��??��2��o
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//���䨬???��??��3��o
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:���䨬???��??��o?��?��?:1~3
//����???��:���䨬???��??��?��
uint8_t HAL_W25QXX_Read_SR(uint8_t regno)
{
	uint8_t byte=0,command=0; 
	switch(regno)
	{
		case 1:
			command=W25X_ReadStatusReg1;    //?�����䨬???��??��1??��?
			break;
		case 2:
			command=W25X_ReadStatusReg2;    //?�����䨬???��??��2??��?
			break;
		case 3:
			command=W25X_ReadStatusReg3;    //?�����䨬???��??��3??��?
			break;
		default:
			command=W25X_ReadStatusReg1;    
			break;
	}    
	HAL_W25QXX_CS_ENABLE();                            //��1?��?��?t   
	HAL_SPI_Send_Byte(command);            //����?��?����?���䨬???��??��?����?    
	byte=HAL_SPI_Receive_Byte();          //?����?��???��??��  
	HAL_W25QXX_CS_DISABLE();                            //��???????     
	return byte;   
}

//D��W25QXX���䨬???��??��
void HAL_W25QXX_Write_SR(uint8_t regno,uint8_t sr)   
{   
	uint8_t command=0;
	switch(regno)
	{
		case 1:
			command=W25X_WriteStatusReg1;    //D����䨬???��??��1??��?
			break;
		case 2:
			command=W25X_WriteStatusReg2;    //D����䨬???��??��2??��?
			break;
		case 3:
			command=W25X_WriteStatusReg3;    //D����䨬???��??��3??��?
			break;
		default:
			command=W25X_WriteStatusReg1;    
			break;
	}   
	HAL_W25QXX_CS_ENABLE();                            //��1?��?��?t   
	HAL_SPI_Send_Byte(command);            //����?��D�䨨?���䨬???��??��?����?    
	HAL_SPI_Send_Byte(sr);                 //D�䨨?��???��??��  
	HAL_W25QXX_CS_DISABLE();                            //��???????     	      
} 
//�̨���y???D
void HAL_W25QXX_Wait_Busy(void)   
{   
	while((HAL_W25QXX_Read_SR(1)&0x01)==0x01);   // �̨���yBUSY??????
}
//W25QXXD�䨺1?��	
//??WEL????   
void HAL_W25QXX_Write_Enable(void)   
{
	HAL_W25QXX_CS_ENABLE();                            //��1?��?��?t   
    HAL_SPI_Send_Byte(W25X_WriteEnable);  				//����?��D�䨺1?��  
	HAL_W25QXX_CS_DISABLE();                           //��???????     	      
} 
//W25QXXD�䨺1?��	
//??WEL????   
void HAL_W25QXX_Write_Disable(void)   
{
	HAL_W25QXX_CS_ENABLE();                            //��1?��?��?t   
  HAL_SPI_Send_Byte(W25X_WriteDisable);  				  //����?��D�䨺1?��  
	HAL_W25QXX_CS_DISABLE();                           //��???????     	      
}
//?����?SPI FLASH  
//?��???����??��?a��??����????��3��?����?��y?Y
//pBuffer:��y?Y��?���??
//ReadAddr:?a��??����?��?��??��(24bit)
//NumByteToRead:��a?����?��?��??����y(��?�䨮65535)
uint8_t HAL_W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	HAL_W25QXX_CS_ENABLE();                          //��1?��?��?t   
	HAL_SPI_Send_Byte(W25X_ReadData);      //����?��?����??����?  
	HAL_SPI_Send_Byte((uint8_t)((ReadAddr)>>16));   //����?��24bit��??��    
	HAL_SPI_Send_Byte((uint8_t)((ReadAddr)>>8));   
	HAL_SPI_Send_Byte((uint8_t)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)
	{ 
		pBuffer[i]=HAL_SPI_Receive_Byte();    //?-?��?����y  
	}
	HAL_W25QXX_CS_DISABLE();  		
	return 0;	
}  
//SPI?����?��3(0~65535)?��D�䨨?��������256??��??����?��y?Y
//?��???����??��?a��?D�䨨?��?�䨮256��??����?��y?Y
//pBuffer:��y?Y��?���??
//WriteAddr:?a��?D�䨨?��?��??��(24bit)
//NumByteToWrite:��aD�䨨?��?��??����y(��?�䨮256),??��y2?��|??3?1y??��3��?���ꨮ����??����y!!!	 
void HAL_W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
  HAL_W25QXX_Write_Enable();                  //SET WEL 
	HAL_W25QXX_CS_ENABLE();                            //��1?��?��?t   
	HAL_SPI_Send_Byte(W25X_PageProgram);   //����?��D�䨰3?����?   
	HAL_SPI_Send_Byte((uint8_t)((WriteAddr)>>16)); //����?��24bit��??��    
	HAL_SPI_Send_Byte((uint8_t)((WriteAddr)>>8));   
	HAL_SPI_Send_Byte((uint8_t)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++)
		HAL_SPI_Send_Byte(pBuffer[i]);//?-?��D�䨺y  
	HAL_W25QXX_CS_DISABLE();                            //��??????? 
	HAL_W25QXX_Wait_Busy();					   //�̨���yD�䨨??����?
} 
//?T?��?��D��SPI FLASH 
//��?D?��������?��D���?��??����??��?����?��y?Y��?2??a0XFF,��??��?����?0XFF��|D�䨨?��?��y?Y??����㨹!
//??��D��??��??��31|?�� 
//?��???����??��?a��?D�䨨????��3��?����?��y?Y,��?��?��a���������??��2?????!
//pBuffer:��y?Y��?���??
//WriteAddr:?a��?D�䨨?��?��??��(24bit)
//NumByteToWrite:��aD�䨨?��?��??����y(��?�䨮65535)
//CHECK OK
void HAL_W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //�̣���3���ꨮ����?��??����y		 	    
	if(NumByteToWrite<=pageremain)
		pageremain=NumByteToWrite;//2?�䨮����256??��??��
	while(1)
	{	   
		HAL_W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)
			break;//D�䨨??����?��?
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	
			NumByteToWrite-=pageremain;			  //??������??-D�䨨?��?��?��??����y
			if(NumByteToWrite>256)
				pageremain=256; //��?��??����?D�䨨?256??��??��
			else 
				pageremain=NumByteToWrite; 	  //2?1?256??��??����?
		}
	}    
} 
//D��SPI FLASH  
//?��???����??��?a��?D�䨨????��3��?����?��y?Y
//??o����y��?2��3y2������!
//pBuffer:��y?Y��?���??
//WriteAddr:?a��?D�䨨?��?��??��(24bit)						
//NumByteToWrite:��aD�䨨?��?��??����y(��?�䨮65535)   
uint8_t W25QXX_BUFFER[4096];		 
uint8_t HAL_W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t* W25QXX_BUF;	  
  W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//����??��??��  
	secoff=WriteAddr%4096;//?������???����???��?
	secremain=4096-secoff;//����??���ꨮ��????�䨮D?   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//2a��?��?
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//2?�䨮����4096??��??��
	while(1) 
	{	
		HAL_W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//?��3?????����??��??����Y
		for(i=0;i<secremain;i++)//D��?����y?Y
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//D����a2��3y  	  
		}
		if(i<secremain)//D����a2��3y
		{
			HAL_W25QXX_Erase_Sector(secpos);//2��3y?a??����??
			for(i=0;i<secremain;i++)	   //?��??
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			HAL_W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//D�䨨?????����??  
		}
		else 
			HAL_W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//D�䨰??-2��3y��?��?,?��?��D�䨨?����??���ꨮ��????. 				   
		if(NumByteToWrite==secremain)
			break;//D�䨨??����?��?
		else//D�䨨??��?����?
		{
			secpos++;//����??��??��??1
			secoff=0;//??��??????a0 	 

			pBuffer+=secremain;  //??????��?
			WriteAddr+=secremain;//D���??��??��?	   
			NumByteToWrite-=secremain;				//��??����y��Y??
			if(NumByteToWrite>4096)secremain=4096;	//??��???����???1��?D��2?����
			else secremain=NumByteToWrite;			//??��???����???����?D�䨪����?
		}	 
	};	
	return 0;	
}
//2��3y????D???		  
//�̨���y����??3?3��...
void HAL_W25QXX_Erase_Chip(void)   
{                                   
	HAL_W25QXX_Write_Enable();                  //SET WEL 
	HAL_W25QXX_Wait_Busy();   
	HAL_W25QXX_CS_ENABLE();                            //��1?��?��?t   
	HAL_SPI_Send_Byte(W25X_ChipErase);        //����?��??2��3y?����?  
	HAL_W25QXX_CS_DISABLE();                            //��???????     	      
	HAL_W25QXX_Wait_Busy();   				   //�̨���yD???2��3y?����?
}
//2��3y��???����??
//Dst_Addr:����??��??�� ?��?Y����?����Y��?����??
//2��3y��???����??��?��?��������??:150ms
void HAL_W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
	//?������falsh2��3y?��??,2a��?��?   
 	//printf("fe:%x\r\n",Dst_Addr);	  
	Dst_Addr*=4096;
	HAL_W25QXX_Write_Enable();                  //SET WEL 	 
	HAL_W25QXX_Wait_Busy();   
	HAL_W25QXX_CS_ENABLE();                            //��1?��?��?t   
	HAL_SPI_Send_Byte(W25X_SectorErase);   //����?������??2��3y??��? 
	HAL_SPI_Send_Byte((uint8_t)((Dst_Addr)>>16));  //����?��24bit��??��    
	HAL_SPI_Send_Byte((uint8_t)((Dst_Addr)>>8));   
	HAL_SPI_Send_Byte((uint8_t)Dst_Addr);  
	HAL_W25QXX_CS_DISABLE();                            //��???????     	      
	HAL_W25QXX_Wait_Busy();   				    //�̨���y2��3y����3��
} 
//??��?��?��??�꨺?
void HAL_W25QXX_PowerDown(void)   
{ 
	HAL_W25QXX_CS_ENABLE();                            //��1?��?��?t   
	HAL_SPI_Send_Byte(W25X_PowerDown);     //����?����?��??����?  
	HAL_W25QXX_CS_DISABLE();                            //��???????     	      
	HAL_Delay(1);                            //�̨���yTPD  
}   
//??D?
void HAL_W25QXX_WAKEUP(void)   
{  
  	HAL_W25QXX_CS_ENABLE();                                //��1?��?��?t   
    HAL_SPI_Send_Byte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB    
	HAL_W25QXX_CS_DISABLE();                               //��???????     	      
    HAL_Delay(1);                                //�̨���yTRES1 ��?��|?������??1ms
}