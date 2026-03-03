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
#define RC522_CS LATAbits.LATA5

uint8_t RC522_Ver;

void initialize()
{
    // Digital pins
    ANSELCbits.ANSC3 = 0;
    ANSELCbits.ANSC5 = 0;
    ANSELCbits.ANSC4 = 0;
    ANSELAbits.ANSA5 = 0;
    ANSELAbits.ANSA4 = 0; // Reset

    // Direction
    TRISCbits.TRISC3 = 0;   // SCK
    TRISCbits.TRISC5 = 0;   // MOSI
    TRISCbits.TRISC4 = 1;   // MISO
    TRISAbits.TRISA5 = 0;   // CS
    TRISAbits.TRISA4 = 0;   // Reset

    // PPS
    SSP1DATPPS = 0x14;  // RC4 -> SDI
    RC5PPS = 0x15;      // SDO1
    RC3PPS = 0x14;      // SCK1

    // SPI Master
    SSP1ADD = 7;
    SSP1STATbits.SMP = 1;
    SSP1STATbits.CKE = 1;
    SSP1CON1bits.CKP = 0;
    SSP1CON1bits.SSPM = 0b1010;
    SSP1CON1bits.SSPEN = 1;

    // Reset RC522
    LATAbits.LATA4 = 0;
    __delay_ms(10);
    LATAbits.LATA4 = 1;
    __delay_ms(50);

    RC522_CS = 1;   // deselect
    
    
    
    // Testing LED
    ANSELAbits.ANSA0 = 0;
    TRISAbits.TRISA0 = 0;
}

uint8_t SPI_WriteByte(uint8_t data)
{
    SSP1BUF = data;
    while(!SSP1STATbits.BF) {};
    return SSP1BUF;
}

uint8_t getVersion()
{
    RC522_CS = 0;
        
    uint8_t command = (0x37 << 1) | 0x80;    
    SPI_WriteByte(command);    // VersionReg read command - Version 82
    RC522_Ver = SPI_WriteByte(0x00);    // receive version
    
    RC522_CS = 1;
}

void main()
{
    initialize();
    __delay_ms(100);

    while(1)
    {
        getVersion();
        __delay_ms(500);
    }
}