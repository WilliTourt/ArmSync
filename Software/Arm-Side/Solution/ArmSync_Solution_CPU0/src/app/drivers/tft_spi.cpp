#include "tft_spi.h"

void TFT_SPI_Init(void) {
    /* TODO: 初始化 SPI 外设 + TFT 复位序列 + ILI9341 初始化 */
}

void TFT_SPI_WriteCmd(uint8_t cmd) {
    /* TODO: DC=0, CS=0, SPI发送, CS=1 */
}

void TFT_SPI_WriteData(uint8_t data) {
    /* TODO: DC=1, CS=0, SPI发送, CS=1 */
}

void TFT_SPI_WriteColor(uint16_t color) {
    /* TODO: 16位颜色 -> 两次 SPI WriteData */
}

void TFT_SPI_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    /* TODO: 设置 ILI9341 的 CASET/PASET 寄存器 */
}

void TFT_SPI_FillScreen(uint16_t color) {
    /* TODO: 全屏填充 */
}

void TFT_SPI_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg) {
    /* TODO: 字符绘制 */
}

int TFT_Touch_ReadXY(uint16_t *x, uint16_t *y) {
    /* TODO: 读取触摸控制器坐标 */
    return 0;
}

int TFT_Touch_IsPressed(void) {
    /* TODO: 检测触摸中断引脚 */
    return 0;
}
