// PIC16F18855 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits (HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1)
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (FSCM timer disabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR pin is Master Clear function)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON       // Brown-out reset enable bits (Brown-out Reset Enabled, SBOREN bit is ignored)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices)
#pragma config ZCD = OFF        // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit can be cleared and set only once in software)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled, SWDTEN is ignored)
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4
#pragma config WRT = OFF        // UserNVM self-write protection bits (Write protection off)
#pragma config SCANE = available// Scanner Enable bit (Scanner module is available for use)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.)

// CONFIG5
#pragma config CP = OFF         // UserNVM Program memory code protection bit (Program Memory code protection disabled)
#pragma config CPD = OFF        // DataNVM code protection bit (Data EEPROM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.



#include <xc.h>
#include <stdio.h>

#define APDS9960_ADDR 0x39

uint8_t who_am_i = 0;

void I2C_Init() {
    // PPS mapping...
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
}

void I2C_Start() {
    SSP1CON2bits.SEN = 1;
    while (SSP1CON2bits.SEN);
}

void I2C_Stop() {
    SSP1CON2bits.PEN = 1;
    while (SSP1CON2bits.PEN);
}

void I2C_Wait() {
    // Wait for MSSP to be idle (checking specific status bits)
    while ((SSP1STAT & 0x04) || (SSP1CON2 & 0x1F));
}

void I2C_Write(uint8_t data) {
    I2C_Wait();
    SSP1BUF = data;
}

void I2C_RepeatedStart() {
    SSP1CON2bits.RSEN = 1; // Use RSEN for repeated start
    while (SSP1CON2bits.RSEN);
}

uint8_t I2C_Read_NACK() {
    uint8_t data;
    SSP1CON2bits.RCEN = 1;
    while (!SSP1STATbits.BF);
    data = SSP1BUF;

    SSP1CON2bits.ACKDT = 1;  // NACK
    SSP1CON2bits.ACKEN = 1;
    while (SSP1CON2bits.ACKEN);

    return data;
}

uint8_t Read_WhoAmI() {
    uint8_t id;

    I2C_Start();
    I2C_Write(APDS9960_ADDR << 1);        // Address + Write bit (0)
    I2C_Write(0x92);                      // Device ID Register
    
    I2C_RepeatedStart();               
    I2C_Write((APDS9960_ADDR << 1) | 1);  // Address + Read bit (1)
    
    id = I2C_Read_NACK();
    I2C_Stop();

    return id;
}

void main() {
    I2C_Init();
    who_am_i = Read_WhoAmI(); // 0xAB
    while(1);
}
