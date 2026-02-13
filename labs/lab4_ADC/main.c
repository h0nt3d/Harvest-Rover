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

uint16_t adc_result = 0;

void main()
{
    // Set RA0 as output for the LED
    TRISAbits.TRISA0 = 0;   // Set RA0 as output (LED)
    LATAbits.LATA0 = 0;

    // Set RA4 as input for the potentiometer
    TRISAbits.TRISA4 = 1;   // Set RA4 as input (potentiometer)
    ANSELAbits.ANSA4 = 1;   // Set RA4 as analog input
 
    // Set up ADC
    ADCON0bits.ADCS = 0b111;  // Fosc/128
    ADREFbits.ADPREF = 0b00;  // Vdd
    ADREFbits.ADNREF = 0;     // AVss
    
    ADPCH = 0x04;  // RA4 as the ADC input channel
    
    ADCON0bits.ADON = 1;  // Turn on ADC Module
    
    ADACQ = 1;
    
    ADCON0bits.ADFRM0 = 1;
    
    // Interrupts
    PEIE = 1;
    GIE = 1;
    PIE1bits.ADIE = 1;
    
    while (1) {
        ADCON0bits.GO_nDONE = 1;
        while (ADCON0bits.GO_nDONE);
    }
}

void __interrupt() ISR()
{
    if (PIR1bits.ADIF) {
        adc_result = (ADRESH << 8) | ADRESL;

        if (adc_result > 0x0200) {
            LATAbits.LATA0 = 1;
        } else {
            LATAbits.LATA0 = 0;
        }

        PIR1bits.ADIF = 0;
        ADCON0bits.GO_nDONE = 1;
    }
}


