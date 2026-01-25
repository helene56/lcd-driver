#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <xc.h>   // for __delay_ms / __delay_us (XC8)

// Pin definitions
#define _XTAL_FREQ 4000000
#define SDA        RA4
#define SCL        RA5

static inline void sda_low()
{
    // write 0 to pin
    SDA = 0;
    // set to output
    TRISAbits.TRISA4 = 0;
    
}

static inline void sda_release_high()
{ 
    // set to input
    TRISAbits.TRISA4 = 1;

}

static inline uint8_t sda_read()
{
    // read sda pin level
    return PORTAbits.RA4;
}

static inline void scl_low()
{
    // write 0 to pin
    SCL = 0;
    // set to output
    TRISAbits.TRISA5 = 0;
    
}

static inline void scl_release_high()
{
    // SDA = 0;              // keep latch low optionally
    // set to input
    TRISAbits.TRISA5 = 1;

}

static inline uint8_t scl_read()
{
    // read scl pin level
    return PORTAbits.RA5;
}

static inline void i2c_delay()
{
    __delay_us(5);
}


static inline void i2c_start()
{

    sda_release_high();
    scl_release_high();
    i2c_delay();

    sda_low();    // START
    i2c_delay();

    scl_low();
    i2c_delay();
    
}


static inline void i2c_stop()
{
    sda_low();
    i2c_delay();

    scl_release_high();
    i2c_delay();

    sda_release_high(); // STOP
    i2c_delay();
}

static inline void i2c_clk_pulse()
{
    scl_release_high();
    while (!scl_read()) { } // optional timeout later
    i2c_delay();

    scl_low();
    i2c_delay();
}

static inline void i2c_write_bit(uint8_t bit)
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


static inline uint8_t i2c_read_bit()
{
    sda_release_high();
    scl_release_high();
    while (!scl_read()) { }
    i2c_delay();

    uint8_t bit = sda_read();
    scl_low();
    i2c_delay();

    return bit;
    
}


static inline uint8_t i2c_write_byte(uint8_t byte)
{
    for (int i = 7; i >= 0; --i) // msb to lsb
    {
        // shift to the next bit 
        uint8_t bit = (byte >> i) & 1;
        i2c_write_bit(bit);
    }


    // Read ACK bit (0 = ACK, 1 = NACK)
    uint8_t ack = i2c_read_bit();
    return (ack == 0);
}

#endif
