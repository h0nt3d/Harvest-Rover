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
// Use project enums instead of #define for ON and OFF.s

#include <xc.h>

#define _XTAL_FREQ 32000000

#define i2cAddress 0x76


void initialize()
{
    SSP2CON1bits.SSPEN = 1; // Enable I2C
    
    // RC3 -> SDA, RC4 -> SCK
    RC3PPS = 0x15;  // Assign SDA (RC3) to I2C SDA
    RC4PPS = 0x14;  // Assign SCK (RC4) to I2C SCK
    
    TRISCbits.TRISC3 = 1;  // Set RC3 as input
    TRISCbits.TRISC4 = 1;  // Set RC4 as input
    
    ANSELCbits.ANSC3 = 0; // Set to digital 
    ANSELCbits.ANSC4 = 0;
    
    SSP2CON1bits.SSPM = 0x08; // Set I2C host mode (Fosc / (4 * (SSP2ADD + 1)))
    SSP2ADD = 19;
    
    
    // LED
    TRISAbits.TRISA0 = 0;
    ANSELAbits.ANSA0 = 0;
    LATAbits.LATA0 == 0;
    
}

void I2C_Start()
{
    SSP2CON2bits.SEN = 1;
    while (SSP2CON2bits.SEN);  // Wait until the SEN bit is cleared
}

void I2C_Write(uint8_t data)
{
    SSP2BUF = data; 
    while (SSP2CON2bits.ACKSTAT);  // Wait for the ACK to be received
}

uint8_t I2C_ACK_Status()
{
    if (SSP2CON2bits.ACKSTAT) {
        return 0;
    }
    else {
        return 1;
    }
}

void I2C_Stop() 
{
    SSP2CON2bits.PEN = 1;
    while (SSP2CON2bits.PEN);
}

void I2C_RepeatedStart() 
{
    SSP2CON2bits.RSEN = 1;
    while (SSP2CON2bits.RSEN);
}

uint8_t I2C_Read()
{
    SSP2CON2bits.RCEN = 1;
    while (!SSP2STATbits.BF);

    return SSP2BUF;
}

void I2C_NACK()
{
    SSP2CON2bits.ACKDT = 1;
    SSP2CON2bits.ACKEN = 1;
    while (SSP2CON2bits.ACKEN);
}

uint8_t BMP280_ReadByte(uint8_t address, uint8_t reg)
{
    uint8_t data;
    
    I2C_Start();  
    I2C_Write(address << 1);
    if (!I2C_ACK_Status()) {
        return 0;  // error
    }

    I2C_Write(reg);
    if (!I2C_ACK_Status()) {
        return 0;  // error
    }

    I2C_RepeatedStart();
    I2C_Write((address << 1) | 1);  // Write address with the read bit (address << 1 | 1)
    if (!I2C_ACK_Status()) {
        return 0;  // error
    }
    
    data = I2C_Read();
    
    I2C_NACK();
    
    
    return data;
}

uint8_t BMP280_WriteByte(uint8_t address, uint8_t reg, uint8_t data)
{
    I2C_Start();  
    I2C_Write(address << 1);  // Shift address to the left and send (write mode)
    if (!I2C_ACK_Status()) {
        return 0;  // error
    }
    
    I2C_Write(reg);
    if (!I2C_ACK_Status()) {
        return 0;  // error
    }
    
    I2C_Write(data);  
    if (!I2C_ACK_Status()) {
        return 0;  // error
    }

    return 1;
}

void main()
{
    initialize();
    uint8_t normal_power = BMP280_WriteByte(i2cAddress, 0xF4, 0x27); // Normal power mode
    if (normal_power == 1) {
        LATAbits.LATA0 == 1;
    }
    else {
        LATAbits.LATA0 == 0;
    }
    
    
    
    uint8_t msb = BMP280_ReadByte(i2cAddress,0xFA);
    uint8_t lsb = BMP280_ReadByte(i2cAddress,0xFB);
    uint8_t xlsb = BMP280_ReadByte(i2cAddress,0xFC);
    
    uint32_t raw_temp = ((uint32_t)msb << 12) | ((uint32_t)lsb << 4) | ((uint32_t)xlsb >> 4);
    uint16_t temp = (uint16_t)raw_temp/16;
            
    while (1) {
        __delay_ms(500);
        uint8_t msb = BMP280_ReadByte(i2cAddress,0xFA);
        uint8_t lsb = BMP280_ReadByte(i2cAddress,0xFB);
        uint8_t xlsb = BMP280_ReadByte(i2cAddress,0xFC);
    }
}
