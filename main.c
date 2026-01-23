/**
 * @file main.c
 * @author helene
 * @date 2026-01-22
 * @brief Main function
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#define _XTAL_FREQ 4000000

#define rotary_CLK RA0
#define rotary_DT  RA1
#define rotary_SW  RA2 
#define INPUT      RA3
#define SDA        RA4
#define SCL        RA5
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
    OPTION_REG = 0b00000111;
    INTCON = 0b11110000;
    

    while (1)
    {

    } // infinite loop

    return 0;
}
