/**
 * @file main.c
 * @author helene
 * @date 2026-01-22
 * @brief Main function
 */

#include <builtins.h>
#include <stdint.h>
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
volatile uint8_t cursor_pos = 1;
volatile uint8_t cursor_update_flag = 0;
volatile uint8_t confirm_update_flag = 0;
volatile uint8_t a = 1;
volatile uint8_t b = 1;
volatile int8_t acc = 0;

volatile uint8_t current_rotary_state = (1 << 1) | 1;
volatile uint8_t previous_rotary_state = (1 << 1) | 1;
static int8_t states[] = {0, -1, 1, 0, 1, 0,
                          0, -1, -1, 0, 0,
                           1, 0, 1, -1, 0};

volatile uint8_t qty_states[] = {0, 0};

volatile uint8_t water_confirm = 0;
volatile uint8_t temp_confirm = 0;
volatile uint8_t row_confirm = 0;
volatile uint8_t select_row = 0;
volatile uint8_t water_qty = 2;
volatile uint8_t temp_qty = 65;
char wat_display[]  = "002";
char temp_display[] = "065";
char empty_val[] = " ";
char confirm_val[] = "x";

void __interrupt() isr() // interrupt vector
{
    if (IOCIF && (IOCAF0 || IOCAF1)) // if IOC set and IOC flag
    {

        // clear flag
        IOCAF0 = 0;
        IOCAF1 = 0;
        IOCIF = 0;

        a = PORTAbits.RA0;
        b = PORTAbits.RA1;
        
        current_rotary_state = (uint8_t)((a << 1) | b);
        // turn clockwise - opposite for anticlockwise
        // pattern
        // 11    01
        // 01    00
        // 00    10
        // 10    11
        // prev, curr
        
        acc += states[(previous_rotary_state << 2) | current_rotary_state];

        if (acc >= 4 && current_rotary_state == ((1 << 1) | 1))
        {
            // cursor_pos = (cursor_pos + 1) & 0x0F;
            if (row_confirm == 1)
            {
                // then we move the quanity of current row
                if (select_row == 0)
                {
                    // water_qty = (water_qty + 1) & 0x14;
                    water_qty++;
                }
                else if (select_row == 1)
                {
                    // temp_qty = (temp_qty + 5) & 0x64;
                    temp_qty += 5;
                }
            }
            else
            {
                // toggle the row value only want row 0 or row 1
                select_row = 1;
            }
            cursor_update_flag = 1;
            acc = 0;
        }
        else if (acc <= -4 && current_rotary_state == ((1 << 1) | 1))
        {
            // cursor_pos = (cursor_pos -1) & 0x0F;
            if (row_confirm == 1)
            {
                // then we move the quanity of current row
                if (select_row == 0)
                {
                    // water_qty = (water_qty - 1) & 0x14;
                    water_qty--;
                }
                else if (select_row == 1)
                {
                    // temp_qty = (temp_qty - 5) & 0x64;
                    temp_qty -= 5;
                }
            }
            else
            {
                // toggle the row value only want row 0 or row 1
                select_row = 0;
            }

            cursor_update_flag = 1;
            acc = 0;
        }
        previous_rotary_state = current_rotary_state;
        
        
        
    }
    if (IOCIF && IOCAF2)
    {
        IOCAF2 = 0;
        IOCIF = 0;
        confirm_update_flag = 1;
        
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
    OPTION_REG = 0b00000111;
    INTCON = 0b10001000; // interrupt on change enable bit set

    IOCAP =  0b00000011; // rising edge
    IOCAF = 0; // flag
    IOCANbits.IOCAN0 = 1; // falling edge        
    IOCANbits.IOCAN1 = 1; // falling edge
    // for switch
    IOCAPbits.IOCAP2 = 1; // rising edge


    LCD1602_init(16, 2);

    LCD1602_setCursor(0, 0);
    LCD1602_print("[ ]");
    LCD1602_setCursor(0, 1);
    LCD1602_print("[ ]");
    // start quantity water
    LCD1602_setCursor(4, 0);
    LCD1602_print(wat_display);
    // start quantity temperature
    LCD1602_setCursor(4, 1);
    LCD1602_print(temp_display);
    LCD1602_setCursor(8, 0);
    LCD1602_print("Vand(dL)");
    LCD1602_setCursor(8, 1);
    LCD1602_print("Temp(");
    LCD1602_setCursor(13, 1);
    char dot = 0xdf;
    LCD1602_print(&dot);
    LCD1602_setCursor(14, 1);
    LCD1602_print("C)");
    LCD1602_setCursor(cursor_pos, 0);
    LCD1602_cursor_blink_on();

    while (1)
    {

        if (cursor_update_flag)
        {
            if (row_confirm == 1)
            {
                // then we update the water/temp quantity display
                if (select_row == 0)
                {
                    // update water qty
                    wat_display[0] = '0' + ((water_qty / 100) % 10);
                    wat_display[1] = '0' + ((water_qty / 10) % 10);
                    wat_display[2] = '0' + (water_qty % 10);
                    LCD1602_setCursor(4, select_row);
                    LCD1602_print(wat_display);
                    
                }
                else if (select_row == 1)
                {
                    // update water qty
                    temp_display[0] = '0' + ((temp_qty / 100) % 10);
                    temp_display[1] = '0' + ((temp_qty / 10) % 10);
                    temp_display[2] = '0' + (temp_qty % 10);
                    LCD1602_setCursor(4, select_row);
                    LCD1602_print(temp_display);
                    
                }
                LCD1602_setCursor(cursor_pos, select_row);


            }
            else
            {
                LCD1602_setCursor(cursor_pos, select_row);
                
            }
            cursor_update_flag = 0;
            __delay_ms(1);

            // LCD1602_setCursor(cursor_pos, 0);
            // cursor_update_flag = 0;
            // __delay_ms(1);
        }
        if (confirm_update_flag)
        {

            if (row_confirm == 0)
            {
                LCD1602_cursor_blink_off();
                row_confirm = 1;
                if (qty_states[select_row])
                {
                    LCD1602_print(empty_val);
                    qty_states[select_row] = 0;
                }
                LCD1602_setCursor(cursor_pos, select_row);
            }
            else if (row_confirm == 1)
            {
                if (!qty_states[select_row])
                {
                    LCD1602_print(confirm_val);
                    qty_states[select_row] = 1;
                }
                else
                {
                    LCD1602_print(empty_val);
                    qty_states[select_row] = 0;
                }
                
                LCD1602_setCursor(cursor_pos, select_row);
                LCD1602_cursor_blink_on();
                row_confirm = 0;
                
                
            }

            
            confirm_update_flag = 0;
            __delay_ms(1);

        }
        

    } // infinite loop

    return 0;
}
