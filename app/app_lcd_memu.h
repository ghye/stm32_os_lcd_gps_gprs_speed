#ifndef __APP_LCD_MENU_H__
#define __APP_LCD_MENU_H__

#include <stdint.h>
#include <stdbool.h>

void app_lcd_menu_init(void);
void app_lcd_menu_insert_yn_question(const char *p, uint16_t index);
bool app_lcd_menu_is_have_yn_question(void);
void app_lcd_menu_load_yn_question(char *p, uint16_t *index);
void app_lcd_main_menu_1(void);
void app_lcd_main_menu_1_flash(void);
void app_lcd_main_menu_2(void);
void app_lcd_main_menu_2_flash(void);
void app_lcd_main_menu_3(void);
void app_lcd_main_menu_3_flash(void);
void app_lcd_menu_l1_yn_question(void);

#endif
