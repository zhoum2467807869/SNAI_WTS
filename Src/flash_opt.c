#include "flash_opt.h"

/*
W25Q128 16M 的容量分256 个块（Block），
每个块大小为 64K 字节，每个块又分16个扇区（Sector），
每个扇区 4K 个字节W25Q128 的最少擦除单位为一个扇区，
也就是每次必须擦4K个字节。操作需要给 W25Q128 开辟一个至4K 的缓存区
SRAM 要求比较高，要求芯片必须4K 以上 SRAM 才能很好的操作
*/
static uint32_t fixStoreNums;
uint8_t W25Q16_Init(void) {
	uint16_t id =0;

	//W25Q16_Configuration(); /* SPI引脚配置及初始化 */
	id = SPI_Flash_ReadID();

	if(id == W25Q16_ID){
		fixStoreNums = 512;
		return 0;
	}else if (id == W25Q128_ID){
		fixStoreNums = 4096;
		return 0;
	}
	 else
		return 1;
}

uint32_t w25qxx_getStoreNum(void){
	return fixStoreNums;
}


/**
 *名称：SPI_Flash_ReadID
 *功能：读取芯片ID  W25X16的ID:0XEF14
 **/
uint16_t SPI_Flash_ReadID(void) {
  W25Q16_CS_LOW();                            //使能器件
  uint8_t temp[2] = {0};/*接收数据*/
  uint16_t ret = 0;
  uint8_t temp_eof[2] = {0xff,0xff};
  uint8_t data[4] = {0x90,0x00,0x00,0x00};/*发送读取ID命令*/
  HAL_SPI_Transmit(&hspi2, (uint8_t *)data, 4, 10);
  HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)temp_eof, (uint8_t *)temp, 2,20); 
  W25Q16_CS_HIGH();                   //取消片选
  ret = temp[0];
  ret <<=8;
  ret |= temp[1];
  printf("获取设备ID：0x%04X\n",ret);/*DEBUG INFO*/
  return ret;
}

/**
 *名称：SPI_Flash_Write_Page
 *输入：WriteAddr 开始写入的地址
 NumByteToWrite 要写入的字节数(最大256)，该数不应该超过该页的剩余字节数！！！
 *输出：pBuffer 数据存储区
 *返回：无
 *功能：在指定地址开始写入最大256字节的数据
 *说明：SPI在一页(0~65535)内写入少于256个字节的数据
 **/

void SPI_Flash_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
	uint16_t i;
	uint8_t temp = W25X_PageProgram;	//写页命令    
	SPI_FLASH_Write_Enable();                  //SET WEL
    W25Q16_CS_LOW();                            //使能器件
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);//发送写页命??
	temp = (uint8_t) ((WriteAddr) >> 16);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);//发??4bit地址
	temp = (uint8_t) ((WriteAddr) >> 8);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);
	temp = (uint8_t) WriteAddr;
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);
	for (i = 0; i < NumByteToWrite; i++)
		HAL_SPI_Transmit(&hspi2, (uint8_t *)pBuffer+i, 1, 10); //循环写数
	W25Q16_CS_HIGH();                   //取消片选
	SPI_Flash_Wait_Busy();					   //等待写入结束
}

/**
 *名称：SPI_Flash_Erase_Chip
 *输入：无
 *输出：无
 *返回：无
 *功能：擦除整个芯片整片擦除时??
 W25X16:25s
 W25X32:40s
 W25X64:40s
 等待时间超长...
 *说明：擦除时间来源于手册，有待验??

 **/
void SPI_Flash_Erase_Chip(void) {
	uint8_t temp = W25X_ChipErase;	//片擦除命??
	SPI_FLASH_Write_Enable();      //SET WEL
	SPI_Flash_Wait_Busy();
	W25Q16_CS_LOW();           //使能器件
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);//发送片擦除命令
	W25Q16_CS_HIGH();                  //取消片选
	SPI_Flash_Wait_Busy();   			//等待芯片擦除结束
}

/**-----------------------------------------------------------------
 * @函数   SPI_FLASH_BufferWrite
 * @功能   向SPI FLASH写入一堆数据，写入的字节数可以大于一页容
 *         Writes block of data to the FLASH. In this function,
 *         the number of WRITE cycles are reduced,
 *         using Page WRITE sequence.
 * @参数   - pBuffer : 指向包含写入数据缓冲器的地址指针
 *             pointer to the buffer  containing the data to be
 *             written to the FLASH.
 *         - WriteAddr : flash的写入地址
 *             FLASH's internal address to write to.
 *         - NumByteToWrite : 写入的字节数
 *             number of bytes to write to the FLASH.
 * @返回
 ***----------------------------------------------------------------*/
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % SPI_FLASH_PageSize;
	count = SPI_FLASH_PageSize - Addr;
	NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
	NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

	if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
	{
		if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
		{
			SPI_Flash_Write_Page(pBuffer, WriteAddr, NumByteToWrite);
		} else /* NumByteToWrite > SPI_FLASH_PageSize */
		{
			while (NumOfPage--) {
				SPI_Flash_Write_Page(pBuffer, WriteAddr, SPI_FLASH_PageSize);
				WriteAddr += SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}

			SPI_Flash_Write_Page(pBuffer, WriteAddr, NumOfSingle);
		}
	} else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
	{
		if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
		{
			if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
			{
				temp = NumOfSingle - count;

				SPI_Flash_Write_Page(pBuffer, WriteAddr, count);
				WriteAddr += count;
				pBuffer += count;

				SPI_Flash_Write_Page(pBuffer, WriteAddr, temp);
			} else {
				SPI_Flash_Write_Page(pBuffer, WriteAddr, NumByteToWrite);
			}
		} else /* NumByteToWrite > SPI_FLASH_PageSize */
		{
			NumByteToWrite -= count;
			NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
			NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

			SPI_Flash_Write_Page(pBuffer, WriteAddr, count);
			WriteAddr += count;
			pBuffer += count;

			while (NumOfPage--) {
				SPI_Flash_Write_Page(pBuffer, WriteAddr, SPI_FLASH_PageSize);
				WriteAddr += SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}

			if (NumOfSingle != 0) {
				SPI_Flash_Write_Page(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
}

/**-----------------------------------------------------------------
 * @函数    SPI_FLASH_SectorErase
 * @功能   擦除SPI FLASH一个扇区的驱动函数
 *         Erases the specified FLASH sector.
 * @参数   SectorAddr: 扇区地址 address of the sector to erase.
 * @返回
 ***----------------------------------------------------------------*/
void SPI_FLASH_SectorErase(uint32_t SectorAddr) {
	SPI_FLASH_Write_Enable();/*spi发送写使能命令*/

	/* 拉低片选信号*/
	W25Q16_CS_LOW();
	/* Send Sector Erase instruction */
	uint8_t temp = W25X_SectorErase;
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);

	/* Send SectorAddr high nibble address byte */
	temp = ((SectorAddr & 0xFF0000) >> 16);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);
	/* Send SectorAddr medium nibble address byte */
	temp = ((SectorAddr & 0xFF00) >> 8);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);	

	/* Send SectorAddr low nibble address byte */
	temp = (SectorAddr & 0xFF);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);		

	/* 拉高片选信号*/
	W25Q16_CS_HIGH();

	/* 等待写入SPI的操作完成*/
	SPI_Flash_Wait_Busy();
}

//SPI_FLASH写使
//将WEL置位
void SPI_FLASH_Write_Enable(void) {
	uint8_t temp = W25X_WriteEnable;
    W25Q16_CS_LOW(); /*使能*/
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);//发送写使能
    W25Q16_CS_HIGH();/*结束使能*/
}

//等待空闲
void SPI_Flash_Wait_Busy(void) {
	while ((SPI_Flash_ReadSR() & 0x01) == 0x01); //等待BUSY位清零
    //printf("IDEL\n");/*r调试用*/
}

uint8_t SPI_Flash_WriteSR(uint16_t data) {
	uint8_t temp = 0x01;//写状态寄存器命令
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);//发送写状态寄存器命令
    temp = (data & 0x00ff);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);//发送一个字节
    temp = (data >> 8);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);//发送一个字节
	return 0;
}
//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4    3    2    1   0
//SPR  RV  TB  BP2  BP1  BP0  WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00

uint8_t SPI_Flash_ReadSR(void) {
	uint8_t byte = 0; /*返回*/
	uint8_t temp = W25X_ReadStatusReg;//读取状态寄存器命令
	uint8_t temp_eof = 0xFF;//读取一个字节命
    W25Q16_CS_LOW(); /*使能*/
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);//发送读取状态寄存器命令
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)&temp_eof, (uint8_t *)&byte, 1,2);//读取一个字节
    W25Q16_CS_HIGH();/*结束使能*/
	return byte;
}

/**
 *名称：SPI_Flash_Read
 *输入：ReadAddr 开始读取的地址(24bit)
 NumByteToRead 要读取的字节数(最大65535)
 *输出: pBuffer 指向的数组
 *返回：无
 *功能：在指定地址开始读取指定长度的数据
 *说明：
 **/

void SPI_Flash_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead) {
	uint16_t i;
    W25Q16_CS_LOW();                            //使能器件
	uint8_t temp = W25X_ReadData;//读取命令
	uint8_t temp_eof = 0xFF;
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);//发送读取命??
	temp = (uint8_t)((ReadAddr) >> 16);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);
	temp = (uint8_t)((ReadAddr) >> 8);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);  //发??4bit地址
	temp = (uint8_t)ReadAddr;
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);
	for (i = 0; i < NumByteToRead; i++) {
		HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)&temp_eof, (uint8_t *)pBuffer+i, 1,20);//读取一个字??循环读数
	}
    W25Q16_CS_HIGH();                            //取消片选
}

