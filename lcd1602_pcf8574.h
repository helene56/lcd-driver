#ifndef LCD1602_PCF8574_H_
#define LCD1602_PCF8574_H_

#include <stdint.h>

void LCD1602_init2(uint8_t cols, uint8_t rows);
void LCD1602_clear(void);
void LCD1602_home(void);

void LCD1602_setCursor(uint8_t col, uint8_t row);
void LCD1602_write_char(char c);
void LCD1602_print(const char *str);

void LCD1602_backlight(uint8_t on);
void LCD1602_cursor_blink_on(void);
void LCD1602_cursor_blink_off(void);


#endif
