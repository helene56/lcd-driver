#ifndef LCD1602_MODULE_H_
#define LCD1602_MODULE_H_

#include <stdint.h>
#include <xc.h>   // for __delay_ms / __delay_us (XC8)

// Your I2C bit-bang functions (declare them here, or include your i2c header instead)
// void i2c_start(void);
// void i2c_stop(void);
// uint8_t i2c_write_byte(uint8_t byte);   // must return 1 = ACK, 0 = NACK

/*!
 * Device 7-bit I2C address
 * Original code used (0x7c>>1) = 0x3E
 */
#define LCD_ADDRESS     (0x7E >> 1)

extern uint8_t _showfunction;

/*!
 * commands
 */
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

/*!
 * flags for display entry mode
 */
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

/*!
 * flags for display on/off control
 */
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

/*!
 * flags for display/cursor shift
 */
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

/*!
 * flags for function set
 */
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x8DOTS 0x00

void LCD1602_init(uint8_t col, uint8_t row);
void begin(uint8_t col, uint8_t row);

void command(uint8_t cmd);
void display(void);
void clear(void);

void write_char(uint8_t data);
void send_string(const char *str);
void setCursor(uint8_t col, uint8_t row);

#endif
