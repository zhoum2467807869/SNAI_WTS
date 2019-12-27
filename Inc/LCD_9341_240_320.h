#pragma once
#include <stdint.h>
#define LCD_X_POINT_NUM     240
#define LCD_Y_POINT_NUM     320


void LCD_Init();
void LCD_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Pixel_Index);
void LCD_draw_rect_1bpp(uint16_t x, uint16_t y, uint16_t xSize, uint16_t ySize, uint8_t BytesPerLine, const uint8_t *pData, int diff, int index0, int index1);
void LCD_draw_rect_16bpp(uint16_t x0, uint16_t y0, uint16_t xSize, uint16_t ySize, const uint16_t *pPixel_data);
void LCD_draw_point(uint16_t x, uint16_t y, uint16_t pixel_index);
uint8_t LCD_read_point(uint16_t x, uint16_t y);
