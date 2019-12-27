/*
*******************************************************************************
* ���ܣ���.c�ļ�Ϊ�о�Բ1.8�����������IC:ST7735S
*       ���ϣ��ṩ�����ӿں�������STemWin GUI����
*       ���£�����SPI��GPIO��FreeRTOS��ʱ����
* 
* ���ڣ�2019-03-15
*******************************************************************************
*/

#include "LCD_9341_240_320.h"

#include "main.h"
#include "stm32f4xx_hal.h"
#include "spi.h"
#include "gpio.h"
#include "cmsis_os.h"

#define DMA_BUFFER_SIZE         1024    //DMA�������Ĵ�С

typedef struct
{
    uint8_t     data[DMA_BUFFER_SIZE];  //����
    uint32_t    length;                 //����
}DMA_buffer_t;
static DMA_buffer_t g_DMA_buffer;       //DMA������


static void SPI_WriteData(uint8_t Data)
{
    SPI_send_byte(Data);
}
static void delay_ms(volatile uint32_t nCount)
{
    osDelay(nCount);
}
static void LCD_WriteIndex(uint8_t Index)
{
    //�ȴ�SPI�ϴη������
    while ( !SPI_is_ready() )
    {
        delay_ms(1);
    }
    LCD_CS_clr();//pa9
    LCD_DC_clr();//���0--д����
    SPI_WriteData(Index);
    LCD_CS_set();
}
static void LCD_WriteData(uint8_t Data)
{
    LCD_CS_clr();
    LCD_DC_set();//���1--д����
    SPI_WriteData(Data);
    LCD_CS_set();
}
static void LCD_WriteData_16Bit(uint16_t Data)
{
    LCD_CS_clr();//CS ���0
    LCD_DC_set();//DS + RS ���1
    SPI_WriteData(Data >> 8);
    SPI_WriteData(Data);
//    LCD_CS_set();
}
static void LCD_Reset()//reset GPIO����
{
    LCD_RES_clr();//���0
    delay_ms(2);
    LCD_RES_set();//���1
    delay_ms(20);
}
void LCD_Init()
{
    LCD_Reset();//
    LCD_WriteIndex(0xCF);  
    LCD_WriteData(0x00); 
    LCD_WriteData(0xD9); 
    LCD_WriteData(0X30); 
     
    LCD_WriteIndex(0xED);  
    LCD_WriteData(0x64); 
    LCD_WriteData(0x03); 
    LCD_WriteData(0X12); 
    LCD_WriteData(0X81); 
     
    LCD_WriteIndex(0xE8);  
    LCD_WriteData(0x85); 
    LCD_WriteData(0x10); 
    LCD_WriteData(0x78); 
     
    LCD_WriteIndex(0xCB);  
    LCD_WriteData(0x39); 
    LCD_WriteData(0x2C); 
    LCD_WriteData(0x00); 
    LCD_WriteData(0x34); 
    LCD_WriteData(0x02); 
     
    LCD_WriteIndex(0xF7);  
    LCD_WriteData(0x20); 
     
    LCD_WriteIndex(0xEA);  
    LCD_WriteData(0x00); 
    LCD_WriteData(0x00); 
     
    LCD_WriteIndex(0xC0);    //Power control 
    LCD_WriteData(0x21);   //VRH[5:0] 
     
    LCD_WriteIndex(0xC1);    //Power control 
    LCD_WriteData(0x12);   //SAP[2:0];BT[3:0] 
     
    LCD_WriteIndex(0xC5);    //VCM control 
    LCD_WriteData(0x32); 
    LCD_WriteData(0x3C); 
     
    LCD_WriteIndex(0xC7);    //VCM control2 
    LCD_WriteData(0XC1); 
     
    LCD_WriteIndex(0x36);    // Memory Access Control 
    LCD_WriteData(0xC8);    //������ʾ��ʽ88 /48 /08 /C8 ����λ����ˢ�·���
     
    LCD_WriteIndex(0x3A);   
    LCD_WriteData(0x55); 

    LCD_WriteIndex(0xB1);   
    LCD_WriteData(0x00);   
    LCD_WriteData(0x18); 
     
    LCD_WriteIndex(0xB6);    // Display Function Control 
    LCD_WriteData(0x0A); 
    LCD_WriteData(0xA2); 

     
     
    LCD_WriteIndex(0xF2);    // 3Gamma Function Disable 
    LCD_WriteData(0x00); 
     
    LCD_WriteIndex(0x26);    //Gamma curve selected 
    LCD_WriteData(0x01); 
     
    LCD_WriteIndex(0xE0);    //Set Gamma 
    LCD_WriteData(0x0F); 
    LCD_WriteData(0x20); 
    LCD_WriteData(0x1E); 
    LCD_WriteData(0x09); 
    LCD_WriteData(0x12); 
    LCD_WriteData(0x0B); 
    LCD_WriteData(0x50); 
    LCD_WriteData(0XBA); 
    LCD_WriteData(0x44); 
    LCD_WriteData(0x09); 
    LCD_WriteData(0x14); 
    LCD_WriteData(0x05); 
    LCD_WriteData(0x23); 
    LCD_WriteData(0x21); 
    LCD_WriteData(0x00); 
     
    LCD_WriteIndex(0XE1);    //Set Gamma 
    LCD_WriteData(0x00); 
    LCD_WriteData(0x19); 
    LCD_WriteData(0x19); 
    LCD_WriteData(0x00); 
    LCD_WriteData(0x12); 
    LCD_WriteData(0x07); 
    LCD_WriteData(0x2D); 
    LCD_WriteData(0x28); 
    LCD_WriteData(0x3F); 
    LCD_WriteData(0x02); 
    LCD_WriteData(0x0A); 
    LCD_WriteData(0x08); 
    LCD_WriteData(0x25); 
    LCD_WriteData(0x2D); 
    LCD_WriteData(0x0F); 
     
    LCD_WriteIndex(0x11);    //Exit Sleep 
    delay_ms(120); 
    LCD_WriteIndex(0x29);    //Display on   at  PAGE 109 
}
static void LCD_set_region(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    LCD_WriteIndex(0x2a);//X�趨
    LCD_WriteData(((x_start) >> 8)&0xFF);
    LCD_WriteData((x_start)&0xFF);
    LCD_WriteData(((x_end) >> 8)&0xFF);
    LCD_WriteData((x_end)&0xFF);//���0xEF

    LCD_WriteIndex(0x2b);//����Y
    LCD_WriteData(((y_start) >> 8)&0xFF);
    LCD_WriteData((y_start)&0xFF);
    LCD_WriteData(((y_end) >> 8)&0xFF);//    ��� ��LCD_WriteData(0x01);LCD_WriteData(0x3F);
    LCD_WriteData((y_end)&0xFF);
}

//DMA��������
static void LCD_DMA_WriteData(const uint16_t *pData, uint32_t length)
{
    DMA_buffer_t *pBuffer = &g_DMA_buffer;
    uint32_t i, byte_cnt = 0;
    
    LCD_CS_clr();
    LCD_DC_set();
    
    //�������ݵ�DMA������
    for (i = 0; i < length; ++i)
    {
        if (byte_cnt >= DMA_BUFFER_SIZE)      //DMA����������
        {
            pBuffer->length = DMA_BUFFER_SIZE;
            SPI_DMA_send(pBuffer->data, pBuffer->length);
            //�ȴ�SPI�ϴη������
            while ( !SPI_is_ready() )
            {
                delay_ms(1);
            }
            byte_cnt = 0;
        }
        pBuffer->data[byte_cnt++] = pData[i] >> 8;//���DMA������
        pBuffer->data[byte_cnt++] = pData[i];
    }
    //DMA����
    pBuffer->length = byte_cnt;
    SPI_DMA_send(pBuffer->data, pBuffer->length);
}

//DMA������ͬ������
static void LCD_DMA_WriteSameData(uint16_t sameData, uint32_t length)
{
    DMA_buffer_t *pBuffer = &g_DMA_buffer;
    uint32_t byte_cnt = 0;
    uint32_t i;
    
    LCD_CS_clr();
    LCD_DC_set();
    
    //�������ݵ�DMA������
    for (i = 0; i < length; ++i)
    {
        if (byte_cnt >= DMA_BUFFER_SIZE)      //DMA����������
        {
            pBuffer->length = DMA_BUFFER_SIZE;
            SPI_DMA_send(pBuffer->data, pBuffer->length);
            //�ȴ�SPI�ϴη������
            while ( !SPI_is_ready() )
            {
                delay_ms(1);
            }
            byte_cnt = 0;
        }
        pBuffer->data[byte_cnt++] = sameData >> 8;
        pBuffer->data[byte_cnt++] = sameData;
    }
    //DMA����
    pBuffer->length = byte_cnt;
    SPI_DMA_send(pBuffer->data, pBuffer->length);
}
//DMA����1bpp����
static void LCD_DMA_Write1bpp(const uint8_t *pData, uint16_t xSize, uint16_t ySize, uint8_t BytesPerLine, int diff, int index0, int index1)
{
    DMA_buffer_t *pBuffer = &g_DMA_buffer;
    uint16_t i, j;
    uint16_t index;
    uint16_t offset;
    uint16_t byte_cnt = 0;
    const uint8_t *pxData;
    
    LCD_CS_clr();
    LCD_DC_set();
    
    //�������ݵ�DMA������
    for (i = 0; i < ySize; ++i)
    {
        offset = diff;
        pxData = pData;
        for (j = 0; j < xSize; ++j)
        {
            if (byte_cnt >= DMA_BUFFER_SIZE)      //DMA����������
            {
                pBuffer->length = DMA_BUFFER_SIZE;
                SPI_DMA_send(pBuffer->data, pBuffer->length);
                //�ȴ�SPI�ϴη������
                while ( !SPI_is_ready() )
                {
                    delay_ms(1);
                }
                byte_cnt = 0;
            }
            index = (*pxData & (0x80 >> offset)) ? index1 : index0;
            pBuffer->data[byte_cnt++] = index >> 8;
            pBuffer->data[byte_cnt++] = index;
            if (++offset >= 8)
            {
                offset = 0;
                pxData++;
            }
        }
        pData += BytesPerLine;
    }
    //DMA����
    pBuffer->length = byte_cnt;
    SPI_DMA_send(pBuffer->data, pBuffer->length);
}

//д��������1bpp��
void LCD_draw_rect_1bpp(uint16_t x, uint16_t y, uint16_t xSize, uint16_t ySize, uint8_t BytesPerLine, const uint8_t *pData, int diff, int index0, int index1)
{
    LCD_set_region(x, y, x+xSize-1, y+ySize-1);
    LCD_WriteIndex(0x2c);
    LCD_DMA_Write1bpp(pData, xSize, ySize, BytesPerLine, diff, index0, index1);
}
//д��������16bpp��
void LCD_draw_rect_16bpp(uint16_t x, uint16_t y, uint16_t xSize, uint16_t ySize, const uint16_t *pPixel_data)
{
    uint32_t size;
    
    LCD_set_region(x, y, x+xSize-1, y+ySize-1);
    LCD_WriteIndex(0x2c);
    size = xSize * ySize;
//    for (uint16_t i = 0; i < size; i++)
//    {
//        LCD_WriteData_16Bit(pPixel_data[i]);
//    }
    LCD_DMA_WriteData(pPixel_data, size);
}
//������䣨ͬһ����ɫ��
void LCD_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Pixel_Index)
{
    uint32_t size;
    
    LCD_set_region(x0, y0, x1, y1);
    LCD_WriteIndex(0x2c);
    size = (x1 - x0 + 1) * (y1 - y0 + 1);
//    for (uint16_t i = 0; i < size; i++)
//    {
//        LCD_WriteData_16Bit(Pixel_Index);
//    }
    LCD_DMA_WriteSameData(Pixel_Index, size);
}
//д��
void LCD_draw_point(uint16_t x, uint16_t y, uint16_t Pixel_Index)
{
    LCD_set_region(x, y, x+1, y+1);
    LCD_WriteIndex(0x2c);
    LCD_WriteData_16Bit(Pixel_Index);
}
//����
uint8_t LCD_read_point(uint16_t x, uint16_t y)
{
    return 0;
}



