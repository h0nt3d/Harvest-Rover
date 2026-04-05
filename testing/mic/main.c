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

#define _XTAL_FREQ        32000000UL
#define SAMPLE_COUNT      128
#define SAMPLE_RATE_HZ    32000UL
#define SAMPLE_PERIOD_US  28

#define MIN_TAU  7
#define MAX_TAU  127

#define MIN_PEAK_AMPLITUDE     8
#define LOCAL_MIN_MARGIN_DIV   12
#define CONFIDENCE_DIV         8

uint16_t adc_buffer[SAMPLE_COUNT];
volatile uint16_t current_hz = 0;

// --------------------------------------------------
// ADC
// --------------------------------------------------
void ADC_Init(void)
{
    TRISAbits.TRISA0 = 1;
    ANSELAbits.ANSA0 = 1;

    ADCON0bits.ADCS = 0b101;   // Fosc/32
    ADREFbits.ADPREF = 0b00;   // VDD
    ADREFbits.ADNREF = 0;      // VSS
    ADPCH = 0x00;              // AN0

    ADCON0bits.ADFRM = 1;      // Right justified
    ADCON0bits.ADON = 1;
}

uint16_t ADC_Read(void)
{
    ADCON0bits.GO_nDONE = 1;
    while (ADCON0bits.GO_nDONE);
    return ((uint16_t)ADRESH << 8) | ADRESL;
}

static uint16_t abs_diff_i16(int16_t a, int16_t b) {
    int16_t d = a - b;
    return (d < 0) ? (uint16_t)(-d) : (uint16_t)d;
}

static uint32_t compute_amdf(uint8_t tau, int16_t mean) {
    uint32_t total_diff = 0;
    uint8_t max_j = SAMPLE_COUNT - tau;
    for (uint8_t j = 0; j < max_j; j++) {
        int16_t s1 = (int16_t)adc_buffer[j] - mean;
        int16_t s2 = (int16_t)adc_buffer[j + tau] - mean;
        int16_t d = s1 - s2;
        total_diff += (d < 0) ? (uint16_t)(-d) : (uint16_t)d;
    }
    return total_diff;
}

void main(void) {
    uint8_t i, tau;
    uint32_t sum;
    int16_t mean;

    TRISAbits.TRISA1 = 0; 
    ADC_Init();

    while (1) {
        sum = 0;
        for (i = 0; i < SAMPLE_COUNT; i++) {
            adc_buffer[i] = ADC_Read();
            sum += adc_buffer[i];
            __delay_us(SAMPLE_PERIOD_US); 
        }
        mean = (int16_t)(sum / SAMPLE_COUNT);

        uint16_t max_val = 0;
        uint16_t min_val = 1023;
        for (i = 0; i < SAMPLE_COUNT; i++) {
            if (adc_buffer[i] > max_val) max_val = adc_buffer[i];
            if (adc_buffer[i] < min_val) min_val = adc_buffer[i];
        }

        if ((max_val - min_val) < MIN_PEAK_AMPLITUDE) {
            current_hz = 0;
            LATAbits.LATA1 = 0;
            __delay_ms(50);
            continue;
        }

        // AMDF Analysis
        uint8_t found_tau = 0;
        uint32_t s_prev, s_curr, s_next;

        s_prev = compute_amdf(MIN_TAU - 1, mean);
        s_curr = compute_amdf(MIN_TAU, mean);

        for (tau = MIN_TAU; tau < MAX_TAU; tau++) {
            s_next = compute_amdf(tau + 1, mean);

            if (s_curr < s_prev && s_curr <= s_next) {
                // Depth check
                if ((s_prev - s_curr) > (s_curr / LOCAL_MIN_MARGIN_DIV)) {
                    found_tau = tau;
                    
                    // Parabolic Interpolation for Sub-Integer Accuracy
                    float delta = (float)((int32_t)s_prev - (int32_t)s_next) / 
                                  (2.0f * (float)(s_prev + s_next - 2 * s_curr));
                    
                    float exact_tau = (float)found_tau + delta;
                    current_hz = (uint16_t)((float)SAMPLE_RATE_HZ / exact_tau) - 110;
                    if (current_hz > 1000 && current_hz < 2000)
                        current_hz -= 100;
                    else if (current_hz > 2000 && current_hz < 3000)
                        current_hz -= 350;
                    else if (current_hz > 3000)
                        current_hz -= 500;
                    break; 
                }
            }
            s_prev = s_curr;
            s_curr = s_next;
        }
        __delay_ms(20);
    }
}