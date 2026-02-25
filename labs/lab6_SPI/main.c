
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

#define _XTAL_FREQ 4000000


void initialize()
{   
    /*
     * SPI Config:
     * SCK - RC3
     * SDO - RC5
     * SDI - RC7
     * CS - RA2
     */
    
    // Set to digital
    ANSELCbits.ANSC3 = 0;
    ANSELCbits.ANSC4 = 0;
    ANSELCbits.ANSC7 = 0;
    ANSELAbits.ANSA2 = 0;
    
    TRISCbits.TRISC3 = 0;   // SCK output
    TRISCbits.TRISC5 = 0;   // SDO output
    TRISCbits.TRISC7 = 1;   // SDI input
    TRISAbits.TRISA2 = 0;   // CS output
   
    SSP1DATPPS = 0x17;  // RC7 -> SDI
    RC5PPS = 0x15; // SDO1
    RC3PPS = 0x14; // SCK1
 
    SSP1ADD = 7;
    SSP1STATbits.SMP = 1;   // Sample at end
    SSP1STATbits.CKE = 1;   // Transmit on active-to-idle
    SSP1CON1bits.CKP = 0;   // Clock idle low
    
    SSP1CON1bits.SSPM = 0b1010;   // SPI Master mode, BRG formula
    SSP1CON1bits.SSPEN = 1; // Enable SPI module
    
    
    ANSELAbits.ANSA5 = 0;
    ANSELAbits.ANSA3 = 0;
    TRISAbits.TRISA5 = 1;
    TRISAbits.TRISA3 = 0;
    
    PEIE = 1;
    GIE = 1;
    
}


void SPIWriteByte(uint8_t data)
{
    SSP1BUF = data;
    while(!SSP1STATbits.BF) {};
}

void writeRegister(uint8_t address, uint8_t data)
{
    LATAbits.LATA2 = 0;  // CS low (select slave)
    SPIWriteByte(address);
    SPIWriteByte(data);
    LATAbits.LATA2 = 1; // CS high (deselect slave)
}

uint8_t SPIReadByte()
{
    SSP1BUF = 0x00;   // Sending dummy byte (0x00)
    while(!SSP1STATbits.BF) {};
    return SSP1BUF;
}

uint8_t readRegister(uint8_t address)
{
    uint8_t controlByte = 0x80 | address;
    LATAbits.LATA2 = 0;  // CS low (select slave)
    SPIWriteByte(controlByte);
    uint8_t data = SPIReadByte();
    LATAbits.LATA2 = 1;  // CS high (deselect slave)
    return data;
}



void main()
{
    initialize();
    while(1) {
        if(PORTAbits.RA5 == 0) {
            LATAbits.LATA3 = 1;
            
            uint8_t msb = readRegister(0xFA);
            uint8_t lsb = readRegister (0xFB);
            uint8_t xlsb = readRegister (0xFC);
            
            uint32_t raw_temp = ((uint32_t)msb << 12) | ((uint32_t)lsb << 4) | ((uint32_t)xlsb >> 4);
            uint16_t temp = (uint16_t)raw_temp/16;
            
            __delay_ms(200);
        }
        else {
            LATAbits.LATA3 = 0;
        }
    }
    
}