#include "flash_opt.h"

/*
W25Q128 16M ��������256 ���飨Block����
ÿ�����СΪ 64K �ֽڣ�ÿ�����ַ�16��������Sector����
ÿ������ 4K ���ֽ�W25Q128 �����ٲ�����λΪһ��������
Ҳ����ÿ�α����4K���ֽڡ�������Ҫ�� W25Q128 ����һ����4K �Ļ�����
SRAM Ҫ��Ƚϸߣ�Ҫ��оƬ����4K ���� SRAM ���ܺܺõĲ���
*/
static uint32_t fixStoreNums;
uint8_t W25Q16_Init(void) {
	uint16_t id =0;

	//W25Q16_Configuration(); /* SPI�������ü���ʼ�� */
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
 *���ƣ�SPI_Flash_ReadID
 *���ܣ���ȡоƬID  W25X16��ID:0XEF14
 **/
uint16_t SPI_Flash_ReadID(void) {
  W25Q16_CS_LOW();                            //ʹ������
  uint8_t temp[2] = {0};/*��������*/
  uint16_t ret = 0;
  uint8_t temp_eof[2] = {0xff,0xff};
  uint8_t data[4] = {0x90,0x00,0x00,0x00};/*���Ͷ�ȡID����*/
  HAL_SPI_Transmit(&hspi2, (uint8_t *)data, 4, 10);
  HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)temp_eof, (uint8_t *)temp, 2,20); 
  W25Q16_CS_HIGH();                   //ȡ��Ƭѡ
  ret = temp[0];
  ret <<=8;
  ret |= temp[1];
  printf("��ȡ�豸ID��0x%04X\n",ret);/*DEBUG INFO*/
  return ret;
}

/**
 *���ƣ�SPI_Flash_Write_Page
 *���룺WriteAddr ��ʼд��ĵ�ַ
 NumByteToWrite Ҫд����ֽ���(���256)��������Ӧ�ó�����ҳ��ʣ���ֽ���������
 *�����pBuffer ���ݴ洢��
 *���أ���
 *���ܣ���ָ����ַ��ʼд�����256�ֽڵ�����
 *˵����SPI��һҳ(0~65535)��д������256���ֽڵ�����
 **/

void SPI_Flash_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
	uint16_t i;
	uint8_t temp = W25X_PageProgram;	//дҳ����    
	SPI_FLASH_Write_Enable();                  //SET WEL
    W25Q16_CS_LOW();                            //ʹ������
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);//����дҳ��??
	temp = (uint8_t) ((WriteAddr) >> 16);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);//��??4bit��ַ
	temp = (uint8_t) ((WriteAddr) >> 8);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);
	temp = (uint8_t) WriteAddr;
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);
	for (i = 0; i < NumByteToWrite; i++)
		HAL_SPI_Transmit(&hspi2, (uint8_t *)pBuffer+i, 1, 10); //ѭ��д��
	W25Q16_CS_HIGH();                   //ȡ��Ƭѡ
	SPI_Flash_Wait_Busy();					   //�ȴ�д�����
}

/**
 *���ƣ�SPI_Flash_Erase_Chip
 *���룺��
 *�������
 *���أ���
 *���ܣ���������оƬ��Ƭ����ʱ??
 W25X16:25s
 W25X32:40s
 W25X64:40s
 �ȴ�ʱ�䳬��...
 *˵��������ʱ����Դ���ֲᣬ�д���??

 **/
void SPI_Flash_Erase_Chip(void) {
	uint8_t temp = W25X_ChipErase;	//Ƭ������??
	SPI_FLASH_Write_Enable();      //SET WEL
	SPI_Flash_Wait_Busy();
	W25Q16_CS_LOW();           //ʹ������
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);//����Ƭ��������
	W25Q16_CS_HIGH();                  //ȡ��Ƭѡ
	SPI_Flash_Wait_Busy();   			//�ȴ�оƬ��������
}

/**-----------------------------------------------------------------
 * @����   SPI_FLASH_BufferWrite
 * @����   ��SPI FLASHд��һ�����ݣ�д����ֽ������Դ���һҳ��
 *         Writes block of data to the FLASH. In this function,
 *         the number of WRITE cycles are reduced,
 *         using Page WRITE sequence.
 * @����   - pBuffer : ָ�����д�����ݻ������ĵ�ַָ��
 *             pointer to the buffer  containing the data to be
 *             written to the FLASH.
 *         - WriteAddr : flash��д���ַ
 *             FLASH's internal address to write to.
 *         - NumByteToWrite : д����ֽ���
 *             number of bytes to write to the FLASH.
 * @����
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
 * @����    SPI_FLASH_SectorErase
 * @����   ����SPI FLASHһ����������������
 *         Erases the specified FLASH sector.
 * @����   SectorAddr: ������ַ address of the sector to erase.
 * @����
 ***----------------------------------------------------------------*/
void SPI_FLASH_SectorErase(uint32_t SectorAddr) {
	SPI_FLASH_Write_Enable();/*spi����дʹ������*/

	/* ����Ƭѡ�ź�*/
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

	/* ����Ƭѡ�ź�*/
	W25Q16_CS_HIGH();

	/* �ȴ�д��SPI�Ĳ������*/
	SPI_Flash_Wait_Busy();
}

//SPI_FLASHдʹ
//��WEL��λ
void SPI_FLASH_Write_Enable(void) {
	uint8_t temp = W25X_WriteEnable;
    W25Q16_CS_LOW(); /*ʹ��*/
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);//����дʹ��
    W25Q16_CS_HIGH();/*����ʹ��*/
}

//�ȴ�����
void SPI_Flash_Wait_Busy(void) {
	while ((SPI_Flash_ReadSR() & 0x01) == 0x01); //�ȴ�BUSYλ����
    //printf("IDEL\n");/*r������*/
}

uint8_t SPI_Flash_WriteSR(uint16_t data) {
	uint8_t temp = 0x01;//д״̬�Ĵ�������
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);//����д״̬�Ĵ�������
    temp = (data & 0x00ff);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);//����һ���ֽ�
    temp = (data >> 8);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);//����һ���ֽ�
	return 0;
}
//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4    3    2    1   0
//SPR  RV  TB  BP2  BP1  BP0  WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00

uint8_t SPI_Flash_ReadSR(void) {
	uint8_t byte = 0; /*����*/
	uint8_t temp = W25X_ReadStatusReg;//��ȡ״̬�Ĵ�������
	uint8_t temp_eof = 0xFF;//��ȡһ���ֽ���
    W25Q16_CS_LOW(); /*ʹ��*/
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 1);//���Ͷ�ȡ״̬�Ĵ�������
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)&temp_eof, (uint8_t *)&byte, 1,2);//��ȡһ���ֽ�
    W25Q16_CS_HIGH();/*����ʹ��*/
	return byte;
}

/**
 *���ƣ�SPI_Flash_Read
 *���룺ReadAddr ��ʼ��ȡ�ĵ�ַ(24bit)
 NumByteToRead Ҫ��ȡ���ֽ���(���65535)
 *���: pBuffer ָ�������
 *���أ���
 *���ܣ���ָ����ַ��ʼ��ȡָ�����ȵ�����
 *˵����
 **/

void SPI_Flash_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead) {
	uint16_t i;
    W25Q16_CS_LOW();                            //ʹ������
	uint8_t temp = W25X_ReadData;//��ȡ����
	uint8_t temp_eof = 0xFF;
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);//���Ͷ�ȡ��??
	temp = (uint8_t)((ReadAddr) >> 16);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);
	temp = (uint8_t)((ReadAddr) >> 8);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);  //��??4bit��ַ
	temp = (uint8_t)ReadAddr;
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&temp, 1, 10);
	for (i = 0; i < NumByteToRead; i++) {
		HAL_SPI_TransmitReceive(&hspi2, (uint8_t *)&temp_eof, (uint8_t *)pBuffer+i, 1,20);//��ȡһ����??ѭ������
	}
    W25Q16_CS_HIGH();                            //ȡ��Ƭѡ
}

