/**
 * @file    tft_spi.h
 * @brief   TFT SPI 触摸屏驱动 (M85)
 */
#ifndef TFT_SPI_H
#define TFT_SPI_H
#include <stdint.h>

void TFT_SPI_Init(void);
void TFT_SPI_WriteCmd(uint8_t cmd);
void TFT_SPI_WriteData(uint8_t data);
void TFT_SPI_WriteColor(uint16_t color);
void TFT_SPI_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void TFT_SPI_FillScreen(uint16_t color);
void TFT_SPI_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg);

/** 触摸读取 */
int TFT_Touch_ReadXY(uint16_t *x, uint16_t *y);
int TFT_Touch_IsPressed(void);

#endif
