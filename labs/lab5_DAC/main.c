
// PIC16F18855 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT1  // Power-up default value for COSC bits (HFINTOSC (1MHz))
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

void initialize() {
    TRISAbits.TRISA0 = 0;
    ANSELAbits.ANSA0 = 1; // Analog
    
    T0CON0bits.T0EN = 1;
    T0CON0bits.T016BIT = 0;
    T0CON1bits.T0CS = 0b010;
    T0CON1bits.T0CKPS = 0b1010;
    TMR0 = 0;
    
    DAC1CON0bits.DAC1EN = 1;  // Enable DAC1 module
    DAC1CON0bits.DAC1PSS1 = 0b00;  // Set DAC reference to VDD and VSS (default)
    DAC1CON0bits.DAC1PSS0 = 0b00;
    DAC1CON0bits.DAC1OE1 = 1;
    DAC1CON0bits.DAC1OE2 = 1;
    
    DAC1CON1 = 0;
    
    // Enable interrupts
    PEIE = 1;
    GIE = 1;
    PIE0bits.TMR0IE = 1;
}

void __interrupt() ISR() 
{
    if (PIR0bits.TMR0IF) {
        TMR0 = 0;
        
        DAC1CON1++;  // Increment DAC output
        if (DAC1CON1 > 31) {
            DAC1CON1 = 0;  // Reset to 0 
        }
        
        LATAbits.LATA0 = !LATAbits.LATA0;
        PIR0bits.TMR0IF = 0;
    }
}

void main() 
{
    initialize();
    while (1) {}
}
