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


#include <xc.h>
#include <stdint.h>
#include <stdlib.h>

#define _XTAL_FREQ 32000000 
#define SAMPLE_COUNT 64
#define SAMPLE_RATE_HZ 8000
#define SAMPLE_PERIOD_US 125

// Global variables
uint16_t adc_buffer[SAMPLE_COUNT];
uint16_t current_hz = 0;

void ADC_Init() {
    TRISAbits.TRISA0 = 1;
    ANSELAbits.ANSA0 = 1;
    
    ADCON0bits.ADCS = 0b101;  
    ADREFbits.ADPREF = 0b00;
    ADREFbits.ADNREF = 0;
    ADPCH = 0x00;
    
    ADCON0bits.ADFRM = 1;
    ADCON0bits.ADON = 1;
}

uint16_t ADC_Read() {
    ADCON0bits.GO_nDONE = 1;
    while (ADCON0bits.GO_nDONE);
    return ((uint16_t)((ADRESH << 8) | ADRESL));
}

void main() {
    TRISAbits.TRISA1 = 0;
    LATAbits.LATA1 = 0;
    ADC_Init();

    while (1) {
        for (uint8_t i = 0; i < SAMPLE_COUNT; i++) {
            adc_buffer[i] = ADC_Read();
            __delay_us(SAMPLE_PERIOD_US); 
        }

        uint32_t min_diff = 0xFFFFFFFF;
        uint8_t best_tau = 0;

        for (uint8_t tau = 8; tau < (SAMPLE_COUNT / 2); tau++) {
            uint32_t total_diff = 0;

            for (uint8_t j = 0; j < (SAMPLE_COUNT - tau); j++) {
                total_diff += abs((int16_t)adc_buffer[j] - (int16_t)adc_buffer[j + tau]);
            }

            if (total_diff < min_diff) {
                min_diff = total_diff;
                best_tau = tau;
            }
        }

        if (best_tau > 0) {
            // Frequency (Hz) = Sample Rate / Period (in samples)
            current_hz = (uint16_t)(SAMPLE_RATE_HZ / best_tau);
        }

        if (current_hz > 250 && current_hz < 275) {
            LATAbits.LATA1 = 1; 
        } else {
            LATAbits.LATA1 = 0;
        }
        
        __delay_ms(50);
    }
}