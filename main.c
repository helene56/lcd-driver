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
#define _XTAL_FREQ 4000000
// TODO: consider using LATA register instead of PORTA
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


void sda_low()
{
    // write 0 to pin
    SDA = 0;
    // set to output
    TRISAbits.TRISA4 = 0;
    
}

void sda_release_high()
{ 
    // set to input
    TRISAbits.TRISA4 = 1;

}

uint8_t sda_read()
{
    // read sda pin level
    return PORTAbits.RA4;
}

void scl_low()
{
    // write 0 to pin
    SCL = 0;
    // set to output
    TRISAbits.TRISA5 = 0;
    
}

void scl_release_high()
{
    // SDA = 0;              // keep latch low optionally
    // set to input
    TRISAbits.TRISA5 = 1;

}

uint8_t scl_read()
{
    // read scl pin level
    return PORTAbits.RA5;
}

void i2c_delay()
{
    __delay_us(5);
}


void i2c_start()
{

    sda_release_high();
    scl_release_high();
    i2c_delay();

    sda_low();    // START
    i2c_delay();

    scl_low();
    i2c_delay();
    
}


void i2c_stop()
{
    sda_low();
    i2c_delay();

    scl_release_high();
    i2c_delay();

    sda_release_high(); // STOP
    i2c_delay();
}

void i2c_clk_pulse()
{
    scl_release_high();
    while (!scl_read()) { } // optional timeout later
    i2c_delay();

    scl_low();
    i2c_delay();
}

void i2c_write_bit(uint8_t bit)
{
    if (bit)
    {
        sda_release_high();
    }
    else
    {
        sda_low();
    }
    i2c_delay();
    i2c_clk_pulse();
}


uint8_t i2c_read_bit()
{
    sda_release_high();
    scl_release_high();
    i2c_delay();

    uint8_t bit = sda_read();
    scl_low();
    i2c_delay();

    return bit;
    
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

    while (1)
    {
        // i2c_start();
        // i2c_stop();
        i2c_write_bit(1);
        i2c_write_bit(0);
        i2c_write_bit(1);
        i2c_write_bit(1);
        i2c_write_bit(1);
        i2c_write_bit(0);

    } // infinite loop

    return 0;
}
