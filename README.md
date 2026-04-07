# Harvest Rover
<img src="https://github.com/h0nt3d/ECE3232-Rover/blob/main/images/banner.png?raw=true" width="800">

*Figure 1 - Rover Image*

The Harvest Rover is a small embedded system designed to participate in a competition known as *The Harvest*. During the competition, Rovers will participate by completing various tasks. These include:
- RFID Sequence 
- Optical Signal Decoding
- Environmental Remediation
- Solar Array
- Laser Turret
- Magnetic Anomaly
- Autonomous Line
- Alien Frequency
- Electrical Conductivity
- IR Detection

This rover is specifically designed to complete 4 tasks:
- RFID Sequence
- Optical Signal Decoding
- Solar Array
- Alien Frequency

# Hardware:
- [MPLAB Xpress Development Board (PIC16F18855)](https://www.microchip.com/en-us/development-tool/dm164140)
- [Snap Programmer](https://www.microchip.com/en-us/development-tool/pg164100)
- PCU - [(microbit)](https://microbit.org/buy/) (Proprietary)
- RCLS - [dsPIC33CK128MP205](https://www.digikey.ca/en/products/detail/microchip-technology/DSPIC33CK128MP205-I-PT/9342075) (Proprietary)
- RFID Reader - [RC522 RFID MODULE](https://www.digikey.ca/en/products/detail/sunfounder/CN0090/18668629?s=N4IgTCBcDaICxwIyILQGEByAGLBOLKGAIiALoC%2BQA)
- Color Sensor - [APDS-9960 - Light, Ambient Sensor Evaluation Board](https://www.digikey.ca/en/products/detail/adafruit-industries-llc/3595/7652603?s=N4IgTCBcDaIIwFYwA4C0YDMBOBqByAIiALoC%2BQA)
- Speaker - [Passive Buzzer](https://www.mouser.com/ProductDetail/Seeed-Studio/107020109?qs=GedFDFLaBXFNSeAbVfqSzw%3D%3D)
- Microphone - [SPW2430 - MEMS Omnidirectional Microphones Audio Evaluation Board](https://www.digikey.ca/en/products/detail/adafruit-industries-llc/2716/5604375?s=N4IgTCBcDaIIwFYwA4C0cDMA2MqByAIiALoC%2BQA)
- Op-Amp - [Standard (General Purpose) Amplifier 2 Circuit Rail-to-Rail 8-PDIP](https://www.digikey.ca/en/products/detail/texas-instruments/TLV2462IP/277538?s=N4IgTCBcDa4JwDYC0BGAHHAzEgrEgcgCIgC6AvkA)
- Bright LED - [C512A-WNN-CZ0B0151-ND](https://www.digikey.ca/en/products/detail/creeled-inc/C512A-WNN-CZ0B0151/2809629?s=N4IgTCBcDaIMIFYCMYCCBaA6gOW%2BuAWgAwBCRSy62AIiALoC%2BQA)
- Driving MOSFET - [2N7000FS-ND](https://www.digikey.ca/en/products/detail/onsemi/2N7000/244278?s=N4IgTCBcDa4HIHYAMKBiBlAtHAIiAugL5A)
- [Saleae Logic Analyzer](https://www.amazon.ca/Logic-Black-Compatible-Ultra-Portable-Frustration/dp/B0749G85W2?source=ps-sl-shoppingads-lpcontext&psc=1&smid=A1UJUJY5V4HL51)
- [Flysky transmitter and receiver](https://www.amazon.ca/FS-i6-Transmitter-Receiver-FS-iA6B-Multirotor/dp/B0BRXSH4GZ?dib=eyJ2IjoiMSJ9.16ma_ytwLV5lHwIYSernybzKBm8Z85JCqtjxeHEqOhqIljZfvtulvHIDqG7ghYo-PpET2V8Haxa8JZQnTg9Smqxkqaf-dgzll2qtinbSof8KghYBrSnOurPqSaTo-5nXPXQGPG2wTlP5UF3v7BusKHoLcJWwCCZ-Ba7CrMjuKijHVMbpDg8UoHgo08imXRVWI-xiKb_0KDa01QJbHshEwD294YwzC9sDGPznktQ2E8I8voBogp5FrEND03aojrSGcyiEEnmFV3fsZOdYKWKXLBU66RG-e-ayPdj7OnRj_NQ.0SwYRXFKrN-C3YhNtB-G6acSH_YEwxvLrXPCtC4unIQ&dib_tag=se&keywords=Flysky&qid=1775529181&sr=8-8)

# Software:
- [MPLAB XC8 Compiler](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers/xc8)
- [MPLAB X IDE](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide)


# Initialization
1. Create a new Project in the MPLAB X IDE
2. Select the PIC16F18855 microcontroller
3. Create new source files in the project directory using repository files

Communication is made to PCU via UART protocol.

# Initialization
The UART Module and Interrupts are initialized for asynchronous serial communication.

**Clock Frequency = 32Mhz**


<img src="https://github.com/h0nt3d/ECE3232-Rover/blob/main/images/baud_rate.png?raw=true" width="400">

*Figure 2 - Baud Rate Formula*

SP1BRG = 68

```c
TX1STAbits.SYNC = 0; // Async Mode

TX1STAbits.BRGH = 1; // High-speed baud rate
BAUD1CONbits.BRG16 = 1;

TX1STAbits.TXEN = 1; // Transmitter and Receiver enabled
RC1STAbits.SPEN = 1;    
RC1STAbits.CREN = 1;

SP1BRGH = 68 // Baud Rate for 115200

// Enable interrupts
PEIE = 1;
GIE = 1;
PIR3bits.RCIF = 0;
PIE3bits.RCIE = 1;
```

# Communication with PCU via payload messages

- **Bytes 1 & 2** - Sync Bytes
- **Bytes 3 & 4** - Type of Message
- **Bytes 5 & 6** - Payload Size

### Usage Example:
```c
void send_set_pcu_info()
{
    for (int i = 0; i < 9; i++) {
        TX1REG = set_pcu_info[i];
        while (!TX1STAbits.TRMT) {} // wait until register is empty
    }
}
```

### **Get PCU Info Command**  <br>
```c
volatile uint8_t get_pcu_info[6] = {0xFE, 0x19, 0x01, 0x04, 0x00, 0x00};
```

**PCU Info Respsone** <br>
12 byte buffer <br>
```c
response = [
SYNC1,
SYNC2,
ID_LSB,
ID_MSB,
SIZE_LSB,
SIZE_MSB,
TEAM_ID,
PLAYER_ID,
HEALTH_LSB,
HEALTH_MSB,
SHIELD_FLAG,
REPAIR_FLAG
]
```

### **Set PCU Info Command**  <br>
```c
volatile uint8_t set_pcu_info[9] = {0xFE, 0x19, 0x03, 0x04, 0x03, 0x00, TEAM_ID (##h), PLAYER_ID (##h), DEVICE_ID (##h)};
```
- TEAM_ID - Team number in hexadecimal
- PLAYER_ID - Group number in hexadecimal
- DEVICE_ID - 1 for Rover, 2 for Harvester


### **Get Flysky Info Command / Get User Data Command**  <br>
```c
volatile uint8_t get_flySky_info[6] = {0xFE, 0x19, 0x01, 0x05, 0x00, 0x00};
```

**PCU Flysky Info Respsone** <br>
26 byte buffer <br>
```c
response = [
SYNC1,
SYNC2,
MSG_ID_LSB,
MSG_ID_MSB,
PAYLOAD_SIZE_LSB,
PAYLOAD_SIZE_MSB,
RJ_X_LSB,    // Right X Joystick
RJ_X_MSB,
RJ_Y_LSB,    // Right Y Joystick
RJ_Y_MSB,
LJ_Y_LSB,    // Left Y Joystick
LY_Y_MSB,
LJ_X_LSB,    // Left X Joystick
LJ_X_MSB,
SWITCH_A_LSB, // Switches
SWITCH_A_MSB,
SWITCH_B_LSB,
SWITCH_B_MSB,
SWITCH_C_LSB,
SWITCH_C_MSB,
SWITCH_D_LSB,
SWITCH_D_MSB,
POTEN_VRA_LSB,    // Potentiometer VRA
POTEN_VRA_MSB,
POTEN_VRB_LSB,    // Potentiometer VRB
POTEN_VRB_MSB,
]
```

### **Set Motor Settings Command**  <br>
```c
volatile uint8_t set_motor_settings[10] = {0xFE, 0x19, 0x01, 0x06, 0x04, 0x00, DIRA, PWMA, DIRB, PWMB};
```
- DIRA - Motor A Directions: 0 - Brake, 1 - Forward,  2 - Backward
- DIRB - Motor B Directions:
- PWMA - Pulse Width Modulation A: 0 - 100
- PWMB - Pulse Width Modulation B

### Special Usage: 
```c
void send_motor_settings(uint8_t dirA, uint8_t pwmA, uint8_t dirB, uint8_t pwmB)
{
    uint8_t msg[10] = {0xFE, 0x19, 0x01, 0x06, 0x04, 0x00, dirA, pwmA, dirB, pwmB};

    for (uint8_t i = 0; i < 10; i++) {
        TX1REG = msg[i];
        while (!TX1STAbits.TRMT) {}
    }
}
```

### **Set Laser Scope Command**  <br>
```c
volatile uint8_t set_laser_scope[7] = {0xFE, 0x19, 0x01, 0x08, 0x01, 0x00, ENABLE};
```
- ENABLE - 1 or 0 (On or Off)

### **Shoot Laser Command**  <br>
```c
volatile uint8_t shoot_laser[7] = {0xFE, 0x19, 0x01, 0x09, 0x01, 0x00, TYPE};
```
- TYPE - 1 or 2 (Low Caliber or High Caliber)

### **Shoot Laser (Request Repair Code)**  <br>
```c
volatile uint8_t request_repair[6] = {0xFE, 0x19, 0x03, 0x09, 0x00, 0x00};
```

### **Shoot Laser (Transmit Repair Code)**  <br>
```c
volatile uint8_t transmit_repair[6] = {0xFE, 0x19, 0x04, 0x09, 0x00, 0x00};
```

### **Surface Exploration Command**  <br>
```c
volatile uint8_t set_surface_exploration[10] = {0xFE, 0x19, 0x01, 0x0A, 0x04, 0x00, TASK_ID_LSB, TASK_ID_MSB, TASK_SPECIFIC_VALUE_LSB, TASK_SPECIFIC_VALUE_MSB};
```
Task IDs:
- 1 - RFID
- 2 - Fundamental Frequency

### Special Usage: 
```c
void send_surface_exploration(uint16_t task_id, uint16_t task_value)
{
    uint8_t msg[10] = {
        0xFE, 0x19,
        0x01, 0x0A,
        0x04, 0x00,
        (uint8_t)(task_id & 0xFF),
        (uint8_t)((task_id >> 8) & 0xFF),
        (uint8_t)(task_value & 0xFF),
        (uint8_t)((task_value >> 8) & 0xFF)
    };

    for (uint8_t i = 0; i < 10; i++) {
        TX1REG = msg[i];
        while (!TX1STAbits.TRMT) {}
    }
}
```

# Subsystems

## RFID Sequence
Using SPI communication, the RC522 antenna is turned on so that it can detect nearby RFID tags. A request (REQA) command is sent to check if any RFID card is present. If present, it responds with an ATQA (Answer to Request). An anti-collision command is sent to retrieve the card's UID. Error Checking is finally done to verify the UID of the card. <br>
**SPI Pins:**
- CS - RB2
- SCK - RB3
- MOSI - RB4
- MISO - RB5
- RST - RA1
```c
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
```
[rfid.c](https://github.com/h0nt3d/Harvest-Rover/blob/main/rfid.c)

## Optical Signal Decoding
Using I2C communication, the APDS9960 color sensor is initialized and reads red, green, blue and clear light values from its registers. The sensor data is combined into 16-bit values for each color channel. Based on the dominant color, a note from the buzzer is played.
- Red - C4
- Green - F4
- Blue - A4

**I2C Pins:**
  - SCL - RC3
  - SDA - RC4
```c
void DelayMicroseconds(uint16_t us) 
{
    while(us--) {
        NOP();
        NOP();
        NOP();
        NOP();
    }
}

void PlayC4(uint16_t duration_ms)
{
    ANSELBbits.ANSB1 = 0;
    TRISBbits.TRISB1 = 0;

    uint16_t half_period = 1104;
    uint32_t cycles = ((uint32_t)duration_ms * 1000UL) / (half_period * 2UL);

    for (uint32_t i = 0; i < cycles; i++) {
        LATBbits.LATB1 = 1;
        DelayMicroseconds(half_period);

        LATBbits.LATB1 = 0;
        DelayMicroseconds(half_period);
    }
}
```
[optical.c](https://github.com/h0nt3d/Harvest-Rover/blob/main/optical.c)

## Solar Array
The Solar Array Activation module is designed to provide sufficient illumination to a solar panel to charge it. It uses a C512A-WNN-CZ0B0151-ND LED and a 2N7000 MOSFET to drive voltage making the LED brighter. After directing sufficient light to the panel, its connected gate will open. <br>
**LED Controlling Pin:** RB0

## Alien Frequency
The PIC measures an input signal from the SPW2430 mic using the ADC and stores 128 samples at a fixed sampling rate. It then calculates the average signal level and checks the signal amplitued to make sure a valid waveform is present. To find fundamental frequency, an AMDF (Amplitude Magnitude Difference Function) is used. It compares the signal to delayed versions of itself over a range of **tau**, and looks for the first strong minimum. A small parabolic interpolation is then applied to improve accuracy. <br>
**Analog Input Pin:** RA0
```c
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
```
[microphone.c](https://github.com/h0nt3d/Harvest-Rover/blob/main/microphone.c)


# Flysky Controls
The Flysky transmitter acts as the main interface for switching between sensing modes, movement and communication tasks. <br>
[main.c](https://github.com/h0nt3d/Harvest-Rover/blob/main/main.c)

<img src="https://github.com/h0nt3d/ECE3232-Rover/blob/main/images/rover.gif?raw=true" alt="Sample Image" width="300" height="300">

*Figure 3 - Rover Movement Demo*

<img src="https://github.com/h0nt3d/ECE3232-Rover/blob/main/images/Flysky.png?raw=true">

*Figure 4 - Flysky Diagram*

| Channel | Control          | Operator Action                                                           | Rover Response                                                                   |
| ------- | ---------------- | ------------------------------------------------------------------------- | -------------------------------------------------------------------------------- |
| 1       | Right Joystick Y | Forward / Back                                                            | Moves Forward or Backward.                                                       |
| 2       | Right Joystick X | Request Repair Code and Transmit Repair Code                              | Furthest Left to request repair code and Furthest Right to transmit repair code. |
| 3       | Left Joystick Y  | Get User Info                                                             | Request User Info                                                                |
| 4       | Left Joystick X  | Left / Right                                                              | Turns Left or Right (Skid Steering).                                             |
| 5       | Switch A         | Solar Array Toggle                                                        | Turns on LED for Solar Array                                                     |
| 6       | Switch B         | Frequency Capture                                                         | Starts audio recording for signal frequency capture.                             |
| 7       | Switch C (3-pos) | Pos 1 – Idle (No function) / Pos 2 – Optical Decoding / Pos 3 – RFID Gate | Switches between idle, Optical Decoding and RFID Gate                   |
| 8       | Switch D         | Laser Toggle                                                              | Turns on auto firing laser.                                                      |
| 9       | VRA              | Send RFID Gate UID                                                        | Send RFID digits to homebase.                                                    |
| 10      | VRB              | Send Freq Value                                                           | Send Fundamental Freq to homebase.                                               |

*Table 1 - Flysky controls*


