#include <xc.h>
#include "init.h"


volatile uint8_t rxCount = 0;
volatile uint8_t rxDone = 0;

volatile uint8_t get_pcu_info[6] = {0xFE, 0x19, 0x01, 0x04, 0x00, 0x00};
volatile uint8_t get_flySky_info[6] = {0xFE, 0x19, 0x01, 0x05, 0x00, 0x00};
volatile uint8_t set_motor_settings[10] = {0xFE, 0x19, 0x01, 0x06, 0x04, 0x00, 0x01, 80, 0x01, 80};

volatile uint8_t get_pcu_info_buf[12];
volatile uint8_t get_flySky_info_buf[26];

void send_get_pcu_info()
{
    for (int i = 0; i < 6; i++) {
        TX1REG = get_pcu_info[i];
        while (!TX1STAbits.TRMT) {} // wait until register is empty
    }
}

void send_get_flySky_info()
{
    for (int i = 0; i < 6; i++) {
        TX1REG = get_flySky_info[i];
        while (!TX1STAbits.TRMT) {}
    }
}

void send_set_motor_settings()
{
    for (int i = 0; i < 10; i++) {
        TX1REG = set_motor_settings[i];
        while (!TX1STAbits.TRMT) {}
    }
}

void send_motor_settings(uint8_t dirA, uint8_t pwmA, uint8_t dirB, uint8_t pwmB)
{
    uint8_t msg[10] = {0xFE, 0x19, 0x01, 0x06, 0x04, 0x00, dirA, pwmA, dirB, pwmB};

    for (uint8_t i = 0; i < 10; i++) {
        TX1REG = msg[i];
        while (!TX1STAbits.TRMT) {}
    }
}



void __interrupt() ISR() 
{
    /*
    if (IOCAFbits.IOCAF5 == 1) {
        
        rxCount = 0;
        rxDone = 0;
        
        //send_get_pcu_info();
        send_set_motor_settings();
        IOCAFbits.IOCAF5 = 0;  // Clear IOC flag
    }
    */
    

    if (PIE3bits.RCIE && PIR3bits.RCIF) {
        // handle overrun
        if (RC1STAbits.OERR) {
            RC1STAbits.CREN = 0;
            RC1STAbits.CREN = 1;
        }

        uint8_t b = RC1REG;    // reading clears RCIF

        if (!rxDone && rxCount < 26) {
            get_flySky_info_buf[rxCount++] = b;
            if (rxCount == 26) rxDone = 1;
        }
    }
}