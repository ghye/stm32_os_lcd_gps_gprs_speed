#ifndef __DRIV_LCD_H__
#define __DRIV_LCD_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

void driv_lcd_init(void);
void driv_lcd_clrram(void);
void driv_lcd_disp_addr(void *buf, uint32_t len, uint8_t row/*行*/, uint8_t column/*列*/);
void driv_lcd_disp_dots_addr(void *buf, uint8_t length, uint8_t width, uint8_t row, uint8_t column);
void driv_lcd_clean_all_dots(void);

/*	DMA		*/
void driv_lcd_tx_init_dma(void);
void driv_lcd_tx_reload_dma(void);
void driv_lcd_tx_dma_enable(void);
void driv_lcd_tx_dma_disable(void);
void driv_lcd_clrram_dma(void);
void driv_lcd_disp_dma(void *buf, uint32_t len, uint8_t row/*行*/, uint8_t column/*列*/);
void driv_lcd_clean_all_dots_dma(void);
void driv_lcd_disp_dots_dma(void *buf, uint8_t length, uint8_t width, uint8_t row, uint8_t column);

#endif

