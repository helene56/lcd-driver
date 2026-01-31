#include "lcd1602_pcf8574.h"
#include "i2c.h"
#include <xc.h>

#define LCD_I2C_ADDR        0x3F   // 7-bit address

// PCF8574 pin mapping (most common)
#define PIN_RS              0x01   // P0
#define PIN_RW              0x02   // P1 (we keep it LOW always)
#define PIN_EN              0x04   // P2
#define PIN_BL              0x08   // P3

// LCD command set
#define LCD_CLEARDISPLAY    0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_FUNCTIONSET     0x20
#define LCD_SETDDRAMADDR    0x80

// flags for display entry mode
#define LCD_ENTRYLEFT       0x02
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON       0x04
#define LCD_CURSOROFF       0x00
#define LCD_BLINKOFF        0x00

// flags for function set
#define LCD_4BITMODE        0x00
#define LCD_2LINE           0x08
#define LCD_1LINE           0x00
#define LCD_5x8DOTS         0x00

#define LCD_CURSORON   0x02
#define LCD_BLINKON    0x01


static uint8_t g_backlight = PIN_BL;
static uint8_t g_numlines = 2;

static void lcd_delay_us(uint16_t us)
{
    while (us--)
    {
        __delay_us(1);
    }
}

static void lcd_delay_ms(uint16_t ms)
{
    while (ms--)
    {
        __delay_ms(1);
    }
}

static uint8_t pcf_write(uint8_t data)
{
    uint8_t ok = 1;

    i2c_start();

    ok &= i2c_write_byte((LCD_I2C_ADDR << 1) | 0);   // write
    ok &= i2c_write_byte(data | g_backlight);

    i2c_stop();

    return ok;
}

static void pulseEnable(uint8_t data)
{
    pcf_write(data | PIN_EN);
    lcd_delay_us(1);

    pcf_write(data & (uint8_t)~PIN_EN);
    lcd_delay_us(50); // command settle time
}

static void write4bits(uint8_t nibble_high, uint8_t mode_rs)
{
    // nibble_high must be in upper bits (D7..D4)
    uint8_t data = (nibble_high & 0xF0) | mode_rs;
    pcf_write(data);
    pulseEnable(data);
}

static void lcd_send(uint8_t value, uint8_t mode_rs)
{
    // High nibble
    write4bits(value & 0xF0, mode_rs);

    // Low nibble
    write4bits((uint8_t)(value << 4) & 0xF0, mode_rs);
}

static void lcd_command(uint8_t cmd)
{
    lcd_send(cmd, 0);
}

static void lcd_data(uint8_t data)
{
    lcd_send(data, PIN_RS);
}

void LCD1602_backlight(uint8_t on)
{
    if (on)
    {
        g_backlight = PIN_BL;
    }
    else
    {
        g_backlight = 0;
    }

    // write current state once so it takes effect immediately
    pcf_write(0x00);
}

void LCD1602_init(uint8_t cols, uint8_t rows)
{
    (void)cols;

    g_numlines = rows;

    // Wait after power-up
    lcd_delay_ms(50);

    // IMPORTANT: init sequence for 4-bit mode
    // We are still in 8-bit mode, so we send only high-nibble "0x3" several times
    write4bits(0x30, 0);
    lcd_delay_ms(5);

    write4bits(0x30, 0);
    lcd_delay_us(150);

    write4bits(0x30, 0);
    lcd_delay_us(150);

    // Switch to 4-bit mode
    write4bits(0x20, 0);
    lcd_delay_us(150);

    // Function set: 4-bit, 2 line, 5x8 dots
    if (rows > 1)
    {
        lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    }
    else
    {
        lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS);
    }

    // Display ON, cursor OFF, blink OFF
    lcd_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKOFF);

    // Clear display
    LCD1602_clear();

    // Entry mode set
    lcd_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
}

void LCD1602_clear(void)
{
    lcd_command(LCD_CLEARDISPLAY);
    lcd_delay_ms(2);
}

void LCD1602_home(void)
{
    lcd_command(LCD_RETURNHOME);
    lcd_delay_ms(2);
}

void LCD1602_write_char(char c)
{
    lcd_data((uint8_t)c);
}

void LCD1602_print(const char *str)
{
    while (*str)
    {
        LCD1602_write_char(*str++);
    }
}

void LCD1602_setCursor(uint8_t col, uint8_t row)
{
    // 16x2 typical row offsets
    uint8_t row_offsets[4] = { 0x00, 0x40, 0x14, 0x54 };

    if (row >= g_numlines)
    {
        row = 0;
    }

    lcd_command(LCD_SETDDRAMADDR | (uint8_t)(col + row_offsets[row]));
}


void LCD1602_cursor_blink_on(void)
{
    lcd_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON);
}

void LCD1602_cursor_blink_off(void)
{
    lcd_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
}

