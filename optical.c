#include <xc.h>

#define _XTAL_FREQ 32000000

#define APDS9960_ADDR 0x39

#define APDS9960_ENABLE_PON 0x01 
#define APDS9960_ENABLE_AEN 0x02

typedef struct {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t clear;
} RGBC_t;

void DelayMicroseconds(uint16_t us) 
{
    while(us--) {
        NOP();
        NOP();
        NOP();
        NOP();
    }
}

void PlayC4(uint16_t duration_ms)
{
    ANSELBbits.ANSB1 = 0;
    TRISBbits.TRISB1 = 0;

    uint16_t half_period = 1104;
    uint32_t cycles = ((uint32_t)duration_ms * 1000UL) / (half_period * 2UL);

    for (uint32_t i = 0; i < cycles; i++) {
        LATBbits.LATB1 = 1;
        DelayMicroseconds(half_period);

        LATBbits.LATB1 = 0;
        DelayMicroseconds(half_period);
    }
}

void PlayF4(uint16_t duration_ms)
{
    ANSELBbits.ANSB1 = 0;
    TRISBbits.TRISB1 = 0;

    uint16_t half_period = 802;
    uint32_t cycles = ((uint32_t)duration_ms * 1000UL) / (half_period * 2UL);

    for (uint32_t i = 0; i < cycles; i++) {
        LATBbits.LATB1 = 1;
        DelayMicroseconds(half_period);

        LATBbits.LATB1 = 0;
        DelayMicroseconds(half_period);
    }
}

void PlayA4(uint16_t duration_ms)
{
    ANSELBbits.ANSB1 = 0;
    TRISBbits.TRISB1 = 0;

    uint16_t half_period = 662;
    uint32_t cycles = ((uint32_t)duration_ms * 1000UL) / (half_period * 2UL);

    for (uint32_t i = 0; i < cycles; i++) {
        LATBbits.LATB1 = 1;
        DelayMicroseconds(half_period);

        LATBbits.LATB1 = 0;
        DelayMicroseconds(half_period);
    }
}

void I2C_Init() 
{
    SSP1CLKPPS = 0x13; // RC3 = SCL input
    RC3PPS = 0x14;     // SCL output
    SSP1DATPPS = 0x14; // RC4 = SDA input
    RC4PPS = 0x15;     // SDA output

    ANSELCbits.ANSC3 = 0;
    ANSELCbits.ANSC4 = 0;

    TRISCbits.TRISC3 = 1;  // SCL input
    TRISCbits.TRISC4 = 1;  // SDA input

    SSP1CON1 = 0x28;   // I2C Master mode enabled
    SSP1STAT = 0x80;   // Slew rate disabled (standard mode)
    SSP1ADD = 79;      // 100 kHz @ 32 MHz clock
    
    
    TRISAbits.TRISA0 = 0; // LED
    ANSELAbits.ANSA0 = 0;
    LATAbits.LATA0 = 0;
}

void I2C_Start() 
{
    SSP1CON2bits.SEN = 1; 
    while (SSP1CON2bits.SEN);
}

void I2C_RepeatedStart() 
{
    SSP1CON2bits.RSEN = 1;
    while (SSP1CON2bits.RSEN);
}

void I2C_Stop() {
    SSP1CON2bits.PEN = 1; 
    while (SSP1CON2bits.PEN);
}

void I2C_Write(uint8_t data) 
{
    SSP1BUF = data;
    while (SSP1STATbits.BF || SSP1STATbits.R_nW);
}

uint8_t I2C_Read(uint8_t ack) 
{
    SSP1CON2bits.RCEN = 1;
    while (!SSP1STATbits.BF);
    uint8_t temp = SSP1BUF;
    SSP1CON2bits.ACKDT = !ack;
    SSP1CON2bits.ACKEN = 1;
    while (SSP1CON2bits.ACKEN);
    return temp;
}

void APDS9960_Init() 
{
    I2C_Start();
    I2C_Write(APDS9960_ADDR << 1);
    I2C_Write(0x80); 
    I2C_Write(0x03); 
    I2C_Stop();

    I2C_Start();
    I2C_Write(APDS9960_ADDR << 1);
    I2C_Write(0x81); 
    I2C_Write(0xC0); 
    I2C_Stop();
}

void APDS9960_Reset() 
{
    // Turn off everything
    I2C_Start();
    I2C_Write(APDS9960_ADDR << 1);
    I2C_Write(0x80);      // ENABLE register
    I2C_Write(0x00);      // Power down
    I2C_Stop();
    __delay_ms(20);

    // Re-enable power
    I2C_Start();
    I2C_Write(APDS9960_ADDR << 1);
    I2C_Write(0x80);
    I2C_Write(APDS9960_ENABLE_PON | APDS9960_ENABLE_AEN);
    I2C_Stop();
    __delay_ms(10);

    //re-set integration time
    I2C_Start();
    I2C_Write(APDS9960_ADDR << 1);
    I2C_Write(0x81);
    I2C_Write(0xC0);
    I2C_Stop();
    __delay_ms(200);
}

RGBC_t APDS9960_ReadColors() 
{
    RGBC_t data;
    uint8_t raw[8];

    I2C_Start();
    I2C_Write(APDS9960_ADDR << 1);
    I2C_Write(0x94);
    
    I2C_RepeatedStart();
    I2C_Write((APDS9960_ADDR << 1) | 1);
    
    for(int i = 0; i < 7; i++) raw[i] = I2C_Read(1);
    raw[7] = I2C_Read(0);
    I2C_Stop();

    data.clear = (raw[1] << 8) | raw[0];
    data.red   = (raw[3] << 8) | raw[2];
    data.green = (raw[5] << 8) | raw[4];
    data.blue  = (raw[7] << 8) | raw[6];
    
    if (data.red > data.green && data.red > data.blue) {
        LATAbits.LATA0 = 1;
        PlayC4(750);
    }
    else {
        LATAbits.LATA0 = 0;
    }
    
    if (data.green > data.red && data.green > data.blue) {
        PlayF4(750);
    }
    else {
        LATAbits.LATA0 = 0;
    }
    
    if (data.blue > data.red && data.blue > data.green) {
        PlayA4(750);
    }
    else {
        LATAbits.LATA0 = 0;
    }

    return data;
}