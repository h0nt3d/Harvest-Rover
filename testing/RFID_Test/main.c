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
#include <stdint.h>

#define _XTAL_FREQ 32000000

#define RC522_CS   LATBbits.LATB2
#define RC522_RST  LATAbits.LATA0

uint8_t RC522_Ver;
uint8_t uid[4];
uint8_t status;

#define CommandReg      0x01
#define ComIrqReg       0x04
#define ErrorReg        0x06
#define Status2Reg      0x08
#define FIFODataReg     0x09
#define FIFOLevelReg    0x0A
#define ControlReg      0x0C
#define BitFramingReg   0x0D
#define ModeReg         0x11
#define TxControlReg    0x14
#define TxASKReg        0x15
#define VersionReg      0x37

#define TModeReg        0x2A
#define TPrescalerReg   0x2B
#define TReloadRegH     0x2C
#define TReloadRegL     0x2D

#define PCD_Idle        0x00
#define PCD_Transceive  0x0C
#define PCD_SoftReset   0x0F

#define PICC_REQA       0x26
#define PICC_ANTICOLL   0x93
#define PICC_ANTICOLL_NV 0x20

#define RC522_OK            0
#define RC522_ERR_TIMEOUT   1
#define RC522_ERR_COMM      2
#define RC522_ERR_PROTOCOL  3
#define RC522_ERR_BCC       4


void initialize(void)
{
    // Digital mode
    ANSELBbits.ANSB2 = 0;   // CS
    ANSELBbits.ANSB3 = 0;   // SCK
    ANSELBbits.ANSB4 = 0;   // MOSI
    ANSELBbits.ANSB5 = 0;   // MISO
    ANSELAbits.ANSA0 = 0;   // RST

    // Directions
    TRISBbits.TRISB2 = 0;   // CS output
    TRISBbits.TRISB3 = 0;   // SCK output
    TRISBbits.TRISB4 = 0;   // MOSI output
    TRISBbits.TRISB5 = 1;   // MISO input
    TRISAbits.TRISA0 = 0;   // RST output

    RC522_CS  = 1;
    RC522_RST = 1;

    // PPS
    SSP1DATPPS = 0x0D;   // RB5 -> SDI
    RB4PPS     = 0x15;   // RB4 -> SDO1
    RB3PPS     = 0x14;   // RB3 -> SCK1

    // SPI mode 0
    SSP1ADD = 63;
    SSP1STATbits.SMP = 1;
    SSP1STATbits.CKE = 1;
    SSP1CON1bits.CKP = 0;
    SSP1CON1bits.SSPM = 0b1010;
    SSP1CON1bits.SSPEN = 1;

    // RC522 reset pin
    RC522_RST = 0;
    __delay_ms(10);
    RC522_RST = 1;
    __delay_ms(50);
}

uint8_t SPI_WriteByte(uint8_t data)
{
    PIR3bits.SSP1IF = 0;
    SSP1BUF = data;
    while (!PIR3bits.SSP1IF) {}
    return SSP1BUF;
}

static uint8_t RC522_ReadReg(uint8_t reg)
{
    uint8_t val;
    RC522_CS = 0;
    SPI_WriteByte((reg << 1) | 0x80);
    val = SPI_WriteByte(0x00);
    RC522_CS = 1;
    return val;
}

static void RC522_WriteReg(uint8_t reg, uint8_t value)
{
    RC522_CS = 0;
    SPI_WriteByte((reg << 1) & 0x7E);
    SPI_WriteByte(value);
    RC522_CS = 1;
}

static void RC522_SetBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp = RC522_ReadReg(reg);
    RC522_WriteReg(reg, tmp | mask);
}

static void RC522_ClearBitMask(uint8_t reg, uint8_t mask)
{
    uint8_t tmp = RC522_ReadReg(reg);
    RC522_WriteReg(reg, tmp & (~mask));
}

uint8_t getVersion(void)
{
    RC522_Ver = RC522_ReadReg(VersionReg);
    return RC522_Ver;
}


static void RC522_AntennaOn(void)
{
    uint8_t temp = RC522_ReadReg(TxControlReg);
    if ((temp & 0x03) != 0x03) {
        RC522_SetBitMask(TxControlReg, 0x03);
    }
}

void RC522_InitChip(void)
{
    RC522_WriteReg(CommandReg, PCD_SoftReset);
    __delay_ms(50);

    RC522_WriteReg(TModeReg, 0x8D);
    RC522_WriteReg(TPrescalerReg, 0x3E);
    RC522_WriteReg(TReloadRegL, 30);
    RC522_WriteReg(TReloadRegH, 0);

    RC522_WriteReg(TxASKReg, 0x40);
    RC522_WriteReg(ModeReg, 0x3D);
    RC522_AntennaOn();
}



uint8_t RC522_Transceive(const uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t validBits)
{
    uint8_t i;
    uint16_t timeout;

    RC522_WriteReg(CommandReg, PCD_Idle);
    RC522_WriteReg(ComIrqReg, 0x7F);          // clear IRQ flags
    RC522_WriteReg(FIFOLevelReg, 0x80);       // flush FIFO

    for (i = 0; i < sendLen; i++) {
        RC522_WriteReg(FIFODataReg, sendData[i]);
    }

    RC522_WriteReg(BitFramingReg, validBits);
    RC522_WriteReg(CommandReg, PCD_Transceive);
    RC522_SetBitMask(BitFramingReg, 0x80);

    timeout = 5000;

    while (--timeout) {
        uint8_t irq = RC522_ReadReg(ComIrqReg);
        if (irq & 0x30) break;
        if (irq & 0x01) return RC522_ERR_TIMEOUT;
    }

    RC522_ClearBitMask(BitFramingReg, 0x80);

    if (timeout == 0) return RC522_ERR_TIMEOUT;

    if (RC522_ReadReg(ErrorReg) & 0x13) {
        return RC522_ERR_COMM;
    }

    {
        uint8_t n = RC522_ReadReg(FIFOLevelReg);
        uint8_t lastBits = RC522_ReadReg(ControlReg) & 0x07;

        if (lastBits) {
            *backLen = (uint8_t)((n - 1u) * 8u + lastBits);
        } else {
            *backLen = (uint8_t)(n * 8u);
        }

        for (i = 0; i < n; i++) {
            backData[i] = RC522_ReadReg(FIFODataReg);
        }
    }

    return RC522_OK;
}


uint8_t RC522_RequestA(uint8_t *atqa)
{
    uint8_t cmd[1] = { PICC_REQA };
    uint8_t resp[2];
    uint8_t bitLen = 0;
    uint8_t status;

    status = RC522_Transceive(cmd, 1, resp, &bitLen, 0x07);
    if (status != RC522_OK) return status;

    if (bitLen != 16) return RC522_ERR_PROTOCOL;
    if (resp[0] != 0x04 || resp[1] != 0x00) return RC522_ERR_PROTOCOL;

    atqa[0] = resp[0];
    atqa[1] = resp[1];
    return RC522_OK;
}

uint8_t RC522_Anticollision(uint8_t *uid4, uint8_t *bcc)
{
    uint8_t cmd[2] = { PICC_ANTICOLL, PICC_ANTICOLL_NV };
    uint8_t resp[5];
    uint8_t bitLen = 0;
    uint8_t status;
    uint8_t calcBcc;

    status = RC522_Transceive(cmd, 2, resp, &bitLen, 0x00);
    if (status != RC522_OK) return status;

    if (bitLen != 40) return RC522_ERR_PROTOCOL;

    uid4[0] = resp[0];
    uid4[1] = resp[1];
    uid4[2] = resp[2];
    uid4[3] = resp[3];
    *bcc    = resp[4];

    calcBcc = uid4[0] ^ uid4[1] ^ uid4[2] ^ uid4[3];
    if (calcBcc != *bcc) return RC522_ERR_BCC;

    return RC522_OK;
}

uint8_t RC522_ReadUID(uint8_t *uid4)
{
    uint8_t atqa[2];
    uint8_t bcc;
    uint8_t status;

    status = RC522_RequestA(atqa);
    if (status != RC522_OK) return status;

    status = RC522_Anticollision(uid4, &bcc);
    return status;
}


void main(void)
{

    initialize();

    if (getVersion() == 0x00) {
        while (1) {}
    }

    RC522_InitChip();

    while (1)
    {
        status = RC522_ReadUID(uid);
    }
}