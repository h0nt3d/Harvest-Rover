#include <xc.h>

void initialize()
{
    // I/O
    //TRISAbits.TRISA0 = 0; //LED
    //ANSELAbits.ANSA0 = 0;

    TRISBbits.TRISB0 = 0;
    ANSELBbits.ANSB0 = 0;
    
    TRISAbits.TRISA5 = 1; //Button
    ANSELAbits.ANSA5 = 0;
    
    // UART
    TX1STAbits.SYNC = 0;
    TX1STAbits.BRGH = 1;  
    TX1STAbits.TXEN = 1;   
    RC1STAbits.SPEN = 1;    
    RC1STAbits.CREN = 1;     
    BAUD1CONbits.BRG16 = 1;
    
    SP1BRGL = 68 & 0xFF;
    SP1BRGH = (68 >> 8) & 0xFF;

    // TX
    TRISCbits.TRISC6 = 0;
    ANSELCbits.ANSC6 = 0;
    RC6PPS = 0x10;  // PPS
    
    // RX
    TRISCbits.TRISC5 = 1;
    ANSELCbits.ANSC5 = 0;
    RXPPS = 0x15;
    
    
    // IOC on RA5
    IOCAFbits.IOCAF5 = 0;
    IOCANbits.IOCAN5 = 1;
    IOCIE = 1;

    // Enable interrupts
    PEIE = 1;
    GIE = 1;
    PIR3bits.RCIF = 0;
    PIE3bits.RCIE = 1;
    
}