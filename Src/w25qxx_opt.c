#include "W25Qxx.h"

static void HAL_W25QXX_CS_ENABLE(void);
static void HAL_W25QXX_CS_DISABLE(void);
static void HAL_SPI_Send_Byte(uint8_t byte);
static uint8_t HAL_SPI_Receive_Byte(void);

void HAL_W25QXX_Wait_Busy(void);
/* USER CODE BEGIN 0 */
uint16_t W25QXX_TYPE=W25Q128;	//Ä¬ÈÏÊÇW25Q128
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
//?¨¢¨¨?D???ID
//¡¤¦Ì???¦Ì¨¨???:				   
//0XEF13,¡À¨ª¨º?D???D¨ªo??aW25Q80  
//0XEF14,¡À¨ª¨º?D???D¨ªo??aW25Q16    
//0XEF15,¡À¨ª¨º?D???D¨ªo??aW25Q32  
//0XEF16,¡À¨ª¨º?D???D¨ªo??aW25Q64 
//0XEF17,¡À¨ª¨º?D???D¨ªo??aW25Q128 	  
//0XEF18,¡À¨ª¨º?D???D¨ªo??aW25Q256
uint32_t Temp = 0;
uint32_t HAL_W25QXX_ReadID(void)
{
  /* ????¡ä?DDFLASH: CS¦Ì¨ª¦Ì??? */
  HAL_W25QXX_CS_ENABLE();
  /* ¡¤¡é?¨ª?¨¹¨¢?¡êo?¨¢¨¨?D???D¨ªo?ID */
  HAL_SPI_Send_Byte(W25X_ManufactDeviceID);
	HAL_SPI_Send_Byte(0x00);
	HAL_SPI_Send_Byte(0x00);
	HAL_SPI_Send_Byte(0x00);
  Temp |= HAL_SPI_Receive_Byte()<<8;
  Temp |= HAL_SPI_Receive_Byte();
  HAL_W25QXX_CS_DISABLE();
  return Temp;
}
//?¨¢¨¨?W25QXX¦Ì?¡Á¡ä¨¬???¡ä??¡Â¡ê?W25QXX¨°?12¨®D3??¡Á¡ä¨¬???¡ä??¡Â
//¡Á¡ä¨¬???¡ä??¡Â1¡êo
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:??¨¨?0,¡Á¡ä¨¬???¡ä??¡Â¡À¡ê?¡è??,??o?WP¨º1¨®?
//TB,BP2,BP1,BP0:FLASH??¨®¨°D¡ä¡À¡ê?¡è¨¦¨¨??
//WEL:D¡ä¨º1?¨¹???¡§
//BUSY:?|¡À¨º????(1,?|;0,???D)
//??¨¨?:0x00
//¡Á¡ä¨¬???¡ä??¡Â2¡êo
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//¡Á¡ä¨¬???¡ä??¡Â3¡êo
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:¡Á¡ä¨¬???¡ä??¡Âo?¡ê?¡¤?:1~3
//¡¤¦Ì???¦Ì:¡Á¡ä¨¬???¡ä??¡Â?¦Ì
uint8_t HAL_W25QXX_Read_SR(uint8_t regno)
{
	uint8_t byte=0,command=0; 
	switch(regno)
	{
		case 1:
			command=W25X_ReadStatusReg1;    //?¨¢¡Á¡ä¨¬???¡ä??¡Â1??¨¢?
			break;
		case 2:
			command=W25X_ReadStatusReg2;    //?¨¢¡Á¡ä¨¬???¡ä??¡Â2??¨¢?
			break;
		case 3:
			command=W25X_ReadStatusReg3;    //?¨¢¡Á¡ä¨¬???¡ä??¡Â3??¨¢?
			break;
		default:
			command=W25X_ReadStatusReg1;    
			break;
	}    
	HAL_W25QXX_CS_ENABLE();                            //¨º1?¨¹?¡Â?t   
	HAL_SPI_Send_Byte(command);            //¡¤¡é?¨ª?¨¢¨¨?¡Á¡ä¨¬???¡ä??¡Â?¨¹¨¢?    
	byte=HAL_SPI_Receive_Byte();          //?¨¢¨¨?¨°???¡Á??¨²  
	HAL_W25QXX_CS_DISABLE();                            //¨¨???????     
	return byte;   
}

//D¡äW25QXX¡Á¡ä¨¬???¡ä??¡Â
void HAL_W25QXX_Write_SR(uint8_t regno,uint8_t sr)   
{   
	uint8_t command=0;
	switch(regno)
	{
		case 1:
			command=W25X_WriteStatusReg1;    //D¡ä¡Á¡ä¨¬???¡ä??¡Â1??¨¢?
			break;
		case 2:
			command=W25X_WriteStatusReg2;    //D¡ä¡Á¡ä¨¬???¡ä??¡Â2??¨¢?
			break;
		case 3:
			command=W25X_WriteStatusReg3;    //D¡ä¡Á¡ä¨¬???¡ä??¡Â3??¨¢?
			break;
		default:
			command=W25X_WriteStatusReg1;    
			break;
	}   
	HAL_W25QXX_CS_ENABLE();                            //¨º1?¨¹?¡Â?t   
	HAL_SPI_Send_Byte(command);            //¡¤¡é?¨ªD¡ä¨¨?¡Á¡ä¨¬???¡ä??¡Â?¨¹¨¢?    
	HAL_SPI_Send_Byte(sr);                 //D¡ä¨¨?¨°???¡Á??¨²  
	HAL_W25QXX_CS_DISABLE();                            //¨¨???????     	      
} 
//¦Ì¨¨¡äy???D
void HAL_W25QXX_Wait_Busy(void)   
{   
	while((HAL_W25QXX_Read_SR(1)&0x01)==0x01);   // ¦Ì¨¨¡äyBUSY??????
}
//W25QXXD¡ä¨º1?¨¹	
//??WEL????   
void HAL_W25QXX_Write_Enable(void)   
{
	HAL_W25QXX_CS_ENABLE();                            //¨º1?¨¹?¡Â?t   
    HAL_SPI_Send_Byte(W25X_WriteEnable);  				//¡¤¡é?¨ªD¡ä¨º1?¨¹  
	HAL_W25QXX_CS_DISABLE();                           //¨¨???????     	      
} 
//W25QXXD¡ä¨º1?¨¹	
//??WEL????   
void HAL_W25QXX_Write_Disable(void)   
{
	HAL_W25QXX_CS_ENABLE();                            //¨º1?¨¹?¡Â?t   
  HAL_SPI_Send_Byte(W25X_WriteDisable);  				  //¡¤¡é?¨ªD¡ä¨º1?¨¹  
	HAL_W25QXX_CS_DISABLE();                           //¨¨???????     	      
}
//?¨¢¨¨?SPI FLASH  
//?¨²???¡§¦Ì??¡¤?a¨º??¨¢¨¨????¡§3¡è?¨¨¦Ì?¨ºy?Y
//pBuffer:¨ºy?Y¡ä?¡ä¡é??
//ReadAddr:?a¨º??¨¢¨¨?¦Ì?¦Ì??¡¤(24bit)
//NumByteToRead:¨°a?¨¢¨¨?¦Ì?¡Á??¨²¨ºy(¡Á?¡ä¨®65535)
uint8_t HAL_W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	uint16_t i;   										    
	HAL_W25QXX_CS_ENABLE();                          //¨º1?¨¹?¡Â?t   
	HAL_SPI_Send_Byte(W25X_ReadData);      //¡¤¡é?¨ª?¨¢¨¨??¨¹¨¢?  
	HAL_SPI_Send_Byte((uint8_t)((ReadAddr)>>16));   //¡¤¡é?¨ª24bit¦Ì??¡¤    
	HAL_SPI_Send_Byte((uint8_t)((ReadAddr)>>8));   
	HAL_SPI_Send_Byte((uint8_t)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)
	{ 
		pBuffer[i]=HAL_SPI_Receive_Byte();    //?-?¡¤?¨¢¨ºy  
	}
	HAL_W25QXX_CS_DISABLE();  		
	return 0;	
}  
//SPI?¨²¨°?¨°3(0~65535)?¨²D¡ä¨¨?¨¦¨´¨®¨²256??¡Á??¨²¦Ì?¨ºy?Y
//?¨²???¡§¦Ì??¡¤?a¨º?D¡ä¨¨?¡Á?¡ä¨®256¡Á??¨²¦Ì?¨ºy?Y
//pBuffer:¨ºy?Y¡ä?¡ä¡é??
//WriteAddr:?a¨º?D¡ä¨¨?¦Ì?¦Ì??¡¤(24bit)
//NumByteToWrite:¨°aD¡ä¨¨?¦Ì?¡Á??¨²¨ºy(¡Á?¡ä¨®256),??¨ºy2?¨®|??3?1y??¨°3¦Ì?¨º¡ê¨®¨¤¡Á??¨²¨ºy!!!	 
void HAL_W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
  HAL_W25QXX_Write_Enable();                  //SET WEL 
	HAL_W25QXX_CS_ENABLE();                            //¨º1?¨¹?¡Â?t   
	HAL_SPI_Send_Byte(W25X_PageProgram);   //¡¤¡é?¨ªD¡ä¨°3?¨¹¨¢?   
	HAL_SPI_Send_Byte((uint8_t)((WriteAddr)>>16)); //¡¤¡é?¨ª24bit¦Ì??¡¤    
	HAL_SPI_Send_Byte((uint8_t)((WriteAddr)>>8));   
	HAL_SPI_Send_Byte((uint8_t)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++)
		HAL_SPI_Send_Byte(pBuffer[i]);//?-?¡¤D¡ä¨ºy  
	HAL_W25QXX_CS_DISABLE();                            //¨¨??????? 
	HAL_W25QXX_Wait_Busy();					   //¦Ì¨¨¡äyD¡ä¨¨??¨¢¨º?
} 
//?T?¨¬?¨¦D¡äSPI FLASH 
//¡À?D?¨¨¡¤¡À¡ê?¨´D¡ä¦Ì?¦Ì??¡¤¡¤??¡ì?¨²¦Ì?¨ºy?Y¨¨?2??a0XFF,¡¤??¨°?¨²¡¤?0XFF¡ä|D¡ä¨¨?¦Ì?¨ºy?Y??¨º¡ì¡ã¨¹!
//??¨®D¡Á??¡¥??¨°31|?¨¹ 
//?¨²???¡§¦Ì??¡¤?a¨º?D¡ä¨¨????¡§3¡è?¨¨¦Ì?¨ºy?Y,¦Ì?¨º?¨°a¨¨¡¤¡À¡ê¦Ì??¡¤2?????!
//pBuffer:¨ºy?Y¡ä?¡ä¡é??
//WriteAddr:?a¨º?D¡ä¨¨?¦Ì?¦Ì??¡¤(24bit)
//NumByteToWrite:¨°aD¡ä¨¨?¦Ì?¡Á??¨²¨ºy(¡Á?¡ä¨®65535)
//CHECK OK
void HAL_W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //¦Ì£¤¨°3¨º¡ê¨®¨¤¦Ì?¡Á??¨²¨ºy		 	    
	if(NumByteToWrite<=pageremain)
		pageremain=NumByteToWrite;//2?¡ä¨®¨®¨²256??¡Á??¨²
	while(1)
	{	   
		HAL_W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)
			break;//D¡ä¨¨??¨¢¨º?¨¢?
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	
			NumByteToWrite-=pageremain;			  //??¨¨£¤¨°??-D¡ä¨¨?¨¢?¦Ì?¡Á??¨²¨ºy
			if(NumByteToWrite>256)
				pageremain=256; //¨°?¡ä??¨¦¨°?D¡ä¨¨?256??¡Á??¨²
			else 
				pageremain=NumByteToWrite; 	  //2?1?256??¡Á??¨²¨¢?
		}
	}    
} 
//D¡äSPI FLASH  
//?¨²???¡§¦Ì??¡¤?a¨º?D¡ä¨¨????¡§3¡è?¨¨¦Ì?¨ºy?Y
//??o¡¥¨ºy¡ä?2¨¢3y2¨´¡Á¡Â!
//pBuffer:¨ºy?Y¡ä?¡ä¡é??
//WriteAddr:?a¨º?D¡ä¨¨?¦Ì?¦Ì??¡¤(24bit)						
//NumByteToWrite:¨°aD¡ä¨¨?¦Ì?¡Á??¨²¨ºy(¡Á?¡ä¨®65535)   
uint8_t W25QXX_BUFFER[4096];		 
uint8_t HAL_W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t* W25QXX_BUF;	  
  W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//¨¦¨¨??¦Ì??¡¤  
	secoff=WriteAddr%4096;//?¨²¨¦¨¨???¨²¦Ì???¨°?
	secremain=4096-secoff;//¨¦¨¨??¨º¡ê¨®¨¤????¡ä¨®D?   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//2a¨º?¨®?
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//2?¡ä¨®¨®¨²4096??¡Á??¨²
	while(1) 
	{	
		HAL_W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//?¨¢3?????¨¦¨¨??¦Ì??¨²¨¨Y
		for(i=0;i<secremain;i++)//D¡ê?¨¦¨ºy?Y
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//D¨¨¨°a2¨¢3y  	  
		}
		if(i<secremain)//D¨¨¨°a2¨¢3y
		{
			HAL_W25QXX_Erase_Sector(secpos);//2¨¢3y?a??¨¦¨¨??
			for(i=0;i<secremain;i++)	   //?¡ä??
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			HAL_W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//D¡ä¨¨?????¨¦¨¨??  
		}
		else 
			HAL_W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//D¡ä¨°??-2¨¢3y¨¢?¦Ì?,?¡À?¨®D¡ä¨¨?¨¦¨¨??¨º¡ê¨®¨¤????. 				   
		if(NumByteToWrite==secremain)
			break;//D¡ä¨¨??¨¢¨º?¨¢?
		else//D¡ä¨¨??¡ä?¨¢¨º?
		{
			secpos++;//¨¦¨¨??¦Ì??¡¤??1
			secoff=0;//??¨°??????a0 	 

			pBuffer+=secremain;  //??????¨°?
			WriteAddr+=secremain;//D¡ä¦Ì??¡¤??¨°?	   
			NumByteToWrite-=secremain;				//¡Á??¨²¨ºy¦ÌY??
			if(NumByteToWrite>4096)secremain=4096;	//??¨°???¨¦¨¨???1¨º?D¡ä2?¨ª¨º
			else secremain=NumByteToWrite;			//??¨°???¨¦¨¨???¨¦¨°?D¡ä¨ª¨º¨¢?
		}	 
	};	
	return 0;	
}
//2¨¢3y????D???		  
//¦Ì¨¨¡äy¨º¡À??3?3¡è...
void HAL_W25QXX_Erase_Chip(void)   
{                                   
	HAL_W25QXX_Write_Enable();                  //SET WEL 
	HAL_W25QXX_Wait_Busy();   
	HAL_W25QXX_CS_ENABLE();                            //¨º1?¨¹?¡Â?t   
	HAL_SPI_Send_Byte(W25X_ChipErase);        //¡¤¡é?¨ª??2¨¢3y?¨¹¨¢?  
	HAL_W25QXX_CS_DISABLE();                            //¨¨???????     	      
	HAL_W25QXX_Wait_Busy();   				   //¦Ì¨¨¡äyD???2¨¢3y?¨¢¨º?
}
//2¨¢3y¨°???¨¦¨¨??
//Dst_Addr:¨¦¨¨??¦Ì??¡¤ ?¨´?Y¨º¦Ì?¨º¨¨Y¨¢?¨¦¨¨??
//2¨¢3y¨°???¨¦¨¨??¦Ì?¡Á?¨¦¨´¨º¡À??:150ms
void HAL_W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
	//?¨¤¨º¨®falsh2¨¢3y?¨¦??,2a¨º?¨®?   
 	//printf("fe:%x\r\n",Dst_Addr);	  
	Dst_Addr*=4096;
	HAL_W25QXX_Write_Enable();                  //SET WEL 	 
	HAL_W25QXX_Wait_Busy();   
	HAL_W25QXX_CS_ENABLE();                            //¨º1?¨¹?¡Â?t   
	HAL_SPI_Send_Byte(W25X_SectorErase);   //¡¤¡é?¨ª¨¦¨¨??2¨¢3y??¨¢? 
	HAL_SPI_Send_Byte((uint8_t)((Dst_Addr)>>16));  //¡¤¡é?¨ª24bit¦Ì??¡¤    
	HAL_SPI_Send_Byte((uint8_t)((Dst_Addr)>>8));   
	HAL_SPI_Send_Byte((uint8_t)Dst_Addr);  
	HAL_W25QXX_CS_DISABLE();                            //¨¨???????     	      
	HAL_W25QXX_Wait_Busy();   				    //¦Ì¨¨¡äy2¨¢3y¨ª¨º3¨¦
} 
//??¨¨?¦Ì?¦Ì??¡ê¨º?
void HAL_W25QXX_PowerDown(void)   
{ 
	HAL_W25QXX_CS_ENABLE();                            //¨º1?¨¹?¡Â?t   
	HAL_SPI_Send_Byte(W25X_PowerDown);     //¡¤¡é?¨ª¦Ì?¦Ì??¨¹¨¢?  
	HAL_W25QXX_CS_DISABLE();                            //¨¨???????     	      
	HAL_Delay(1);                            //¦Ì¨¨¡äyTPD  
}   
//??D?
void HAL_W25QXX_WAKEUP(void)   
{  
  	HAL_W25QXX_CS_ENABLE();                                //¨º1?¨¹?¡Â?t   
    HAL_SPI_Send_Byte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB    
	HAL_W25QXX_CS_DISABLE();                               //¨¨???????     	      
    HAL_Delay(1);                                //¦Ì¨¨¡äyTRES1 ¡ä?¡ä|?¨®¨º¡À??1ms
}