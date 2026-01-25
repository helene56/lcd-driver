/**
 * @file main.c
 * @author helene
 * @date 2026-01-22
 * @brief Main function
 */

#include <builtins.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "lcd1602_pcf8574.h"

#define _XTAL_FREQ 4000000
// TODO: consider using LATA register instead of PORTA
#define rotary_CLK RA0
#define rotary_DT  RA1
#define rotary_SW  RA2 
#define INPUT      RA3
/////////////////////////////////////////////////////////configuraçôes//////////////////////////////////////////////////
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
// No CPD config for PIC12F1571

volatile uint8_t counter = 0;

void __interrupt() isr() // interrupt vector
{
    
}


int main()
{

    TRISA = 0b111111;   // set all pins to inputs
    CM1CON0 = 0x00;     // disable comparator 1
    ANSELA = 0;         // no analog ports
    WPUA = 0b000000;    // no weak pull ups
    TMR0 = 0;
    OSCCON = 0b11111111; // internal oscillator to max frequency
    // OSCCON = 0b01101010;
    OPTION_REG = 0b00000111;
    // INTCON = 0b11110000;
    INTCON = 0;
    // try setting sda to output
    // TRISAbits.TRISA4 = 0;
    // uint8_t test_byte = 0b00101110;
    // LCD1602_init(16,2);
    LCD1602_init2(16, 2);

    

    // LCD1602_setCursor(5, 1);        // move cursor somewhere visible
    // LCD1602_cursor_blink_on();      // enable blinking cursor

    // __delay_ms(10);
    // LCD_Init(0x7E); // Initialize LCD module with I2C address = 0x4E
    
    // LCD_Set_Cursor(1, 1);
    // LCD_Write_String(" Khaled Magdy");
    // LCD_Set_Cursor(2, 1);
    // LCD_Write_String(" DeepBlue");
    LCD1602_setCursor(0, 0);
    LCD1602_print("hello");
    while (1)
    {
        // i2c_start();
        // i2c_stop();
        // i2c_write_byte(test_byte);
        // setCursor(0,0);
        // send_string("hello");
        

    } // infinite loop

    return 0;
}
