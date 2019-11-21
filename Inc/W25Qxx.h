#ifndef _W25QXX_H_
#define _W25QXX_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
#define W25Q256 0XEF18

#define W25Q64_CS_Pin GPIO_PIN_12
#define W25Q64_CS_GPIO_Port GPIOB
#define SPI2_CLK_Pin GPIO_PIN_13
#define SPI2_CLK_GPIO_Port GPIOB
#define SPI2_MISO_Pin GPIO_PIN_14
#define SPI2_MISO_GPIO_Port GPIOB
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB

#define   FLASH_SECTOR_SIZE      512
#define   FLASH_BLOCK_SIZE       1 
#define   FLASH_SECTOR_COUNT   (uint32_t)((8*1024*1024)/FLASH_SECTOR_SIZE)
	 
extern uint16_t W25QXX_TYPE;					//∂®“ÂW25QXX–æ∆¨–Õ∫≈		 
	 
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9
/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi2;

extern void _Error_Handler(char *, int);


uint32_t HAL_W25QXX_ReadID(void);
uint8_t HAL_W25QXX_Read_SR(uint8_t regno);
void HAL_W25QXX_Write_SR(uint8_t regno,uint8_t sr);
void HAL_W25QXX_Write_Enable(void);
void HAL_W25QXX_Write_Disable(void);
uint8_t HAL_W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);
uint8_t HAL_W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void HAL_W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void HAL_W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);   
void HAL_W25QXX_Erase_Chip(void);
void HAL_W25QXX_Erase_Sector(uint32_t Dst_Addr);
void HAL_W25QXX_PowerDown(void);
void HAL_W25QXX_WAKEUP(void);

uint8_t w25qxxInit(void);
	
#endif

