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

void ADC_Init()
{
    TRISAbits.TRISA0 = 1;
    ANSELAbits.ANSA0 = 1;

    ADCON0bits.ADCS = 0b101;   // Fosc/32
    ADREFbits.ADPREF = 0b00;   // VDD
    ADREFbits.ADNREF = 0;      // VSS
    ADPCH = 0x00;              // AN0

    ADCON0bits.ADFRM = 1;      // Right justified
    ADCON0bits.ADON = 1;
    
    TRISAbits.TRISA1 = 0;
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

void SPW_sample() {
    uint8_t i, tau;
    uint32_t sum = 0;
    int16_t mean;
    
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
        //continue;
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
                current_hz =
                    (uint16_t)((float)SAMPLE_RATE_HZ / exact_tau) - 110;
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
