#include "lcd1602_module.h"
#include "i2c.h"

uint8_t _showfunction;
uint8_t _showcontrol;
uint8_t _showmode;
uint8_t _numlines;
uint8_t _currline;

// ---- local delay helper (ms) ----
static void lcd_delay_ms(uint16_t ms)
{
    while (ms--)
    {
        __delay_ms(1);
    }
}

// ---- low-level I2C write: send 2 bytes in one I2C transaction ----
// This replaces: write(fd, val, 2);
static uint8_t lcd_i2c_write2(uint8_t b0, uint8_t b1)
{
    uint8_t ok = 1;

    i2c_start();

    // address byte: (7-bit addr << 1) | write(0)
    ok &= i2c_write_byte((LCD_ADDRESS << 1) | 0);

    ok &= i2c_write_byte(b0);
    ok &= i2c_write_byte(b1);

    i2c_stop();

    return ok; // 1 = success (all ACK), 0 = something NACKed
}

void LCD1602_init(uint8_t col, uint8_t row)
{
    _showfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    begin(col, row);
}

void begin(uint8_t col, uint8_t lines)
{
    if (lines > 1)
    {
        _showfunction |= LCD_2LINE;
    }

    _numlines = lines;
    _currline = 0;

    // Wait after power-up (original code waited 50ms)
    lcd_delay_ms(50);

    // Function set sequence (same as original)
    command(LCD_FUNCTIONSET | _showfunction);
    lcd_delay_ms(10);

    command(LCD_FUNCTIONSET | _showfunction);
    lcd_delay_ms(10);

    command(LCD_FUNCTIONSET | _showfunction);

    // Display on, cursor off, blink off
    _showcontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();

    // Clear display
    clear();

    // Entry mode set
    _showmode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    command(LCD_ENTRYMODESET | _showmode);
}

void command(uint8_t cmd)
{
    // control byte 0x80 = command
    // data byte = cmd
    lcd_i2c_write2(0x80, cmd);
}

void display(void)
{
    command(LCD_DISPLAYCONTROL | _showcontrol);
    lcd_delay_ms(2);
}

void clear(void)
{
    command(LCD_CLEARDISPLAY);
    lcd_delay_ms(2);
}

void write_char(uint8_t data)
{
    // control byte 0x40 = data
    lcd_i2c_write2(0x40, data);
}

void send_string(const char *str)
{
    uint8_t i;
    for (i = 0; str[i] != '\0'; i++)
    {
        write_char((uint8_t)str[i]);
    }
}

void setCursor(uint8_t col, uint8_t row)
{
    if (row == 0)
    {
        col |= 0x80;
    }
    else
    {
        col |= 0xC0;
    }
    command(col);
}
