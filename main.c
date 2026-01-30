/**
 * @file main.c
 * @author helene
 * @date 2026-01-22
 * @brief Main function
 */

#include <builtins.h>
// #include <pic12f1571.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "lcd1602_pcf8574.h"

#define _XTAL_FREQ 4000000
// TODO: consider using LATA register instead of PORTA
#define rotary_CLK RA0 // A
#define rotary_DT  RA1 // B
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
volatile uint8_t cursor_pos = 9;
volatile uint8_t cursor_update_flag = 0;
volatile uint8_t a = 1;
volatile uint8_t b = 1;
volatile int8_t acc = 0;

volatile uint8_t current_rotary_state = (1 << 1) | 1;
volatile uint8_t previous_rotary_state = (1 << 1) | 1;

void __interrupt() isr() // interrupt vector
{
    if (IOCIF && (IOCAF)) // if IOC set and IOC flag
    {
        // if (PORTAbits.RA0 != PORTAbits.RA1) // B != A
        // {
        //     // knob is turned clockwise
        //     cursor_pos++;
        //     cursor_pos = cursor_pos % 16; // 16  columns
        //     cursor_update_flag = 1;
            
        // }
        // else if (PORTAbits.RA0 == PORTAbits.RA1)
        // {
        //     // knob is turned counterclockwise
        //     cursor_pos--;
        //     cursor_pos = cursor_pos % 16; // 16  columns
        //     cursor_update_flag = 1;
        // }
        // else if (!PORTAbits.RA0)
        // {
        //     // knob is turned counterclockwise
        //     cursor_pos++;
        //     cursor_pos = cursor_pos % 16; // 16  columns
        //     cursor_update_flag = 1;
        // }
        a = PORTAbits.RA0;
        b = PORTAbits.RA1;
        
        current_rotary_state = (uint8_t)((a << 1) | b);
        // turn clockwise
        if (previous_rotary_state == ((1 << 1) | 1) && current_rotary_state == ((0 << 1) | 1))
        {
            acc++;
        }
        else if (previous_rotary_state == ((0 << 1) | 1) && current_rotary_state == ((0 << 1) | 0))
        {
            acc++;
        }
        else if (previous_rotary_state == ((0 << 1) | 0) && current_rotary_state == ((1 << 1) | 0))
        {
            acc++;
        }
        else if (previous_rotary_state == ((1 << 1) | 0) && current_rotary_state == ((1 << 1) | 1))
        {
            acc++;
        }

        if (acc >= 4 && current_rotary_state == ((1 << 1) | 1))
        {
            cursor_pos++;
            if (cursor_pos > 15)
            {
                cursor_pos = 0;
            }
            cursor_update_flag = 1;
            acc = 0;
        }
        previous_rotary_state = current_rotary_state;
        
        // clear flag
        IOCAF = 0;
        IOCIF = 0;
        
    }
}


int main()
{

    TRISA = 0b111111;   // set all pins to inputs
    CM1CON0 = 0x00;     // disable comparator 1
    ANSELA = 0;         // no analog ports
    WPUA = 0b00000000;    //  weak pull ups
    TMR0 = 0;
    OSCCON = 0b11111111; // internal oscillator to max frequency
    // OSCCON = 0b01101010;
    OPTION_REG = 0b00000111;
    // INTCON = 0b11110000;
    INTCON = 0b10001000; // interrupt on change enable bit set

    IOCAP =  0b00000011;
    IOCAF = 0; // flag
    IOCANbits.IOCAN0 = 1;        
    IOCANbits.IOCAN1 = 1;
    // try setting sda to output
    // TRISAbits.TRISA4 = 0;
    // uint8_t test_byte = 0b00101110;
    // LCD1602_init(16,2);
    LCD1602_init(16, 2);

    

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
    LCD1602_setCursor(cursor_pos, 0);
    LCD1602_cursor_blink_on();
    while (1)
    {
        // i2c_start();
        // i2c_stop();
        // i2c_write_byte(test_byte);
        // setCursor(0,0);
        // send_string("hello");
        if (cursor_update_flag)
        {
            LCD1602_setCursor(cursor_pos, 0);
            cursor_update_flag = 0;
        }
        

    } // infinite loop

    return 0;
}
