# Harvest Rover
<img src="https://github.com/h0nt3d/ECE3232-Rover/blob/main/images/prototype_4.jpg?raw=true" width="700">

## Hardware:
- [MPLAB Xpress Development Board (PIC16F18855)](https://www.microchip.com/en-us/development-tool/dm164140)
- [Snap Programmer](https://www.microchip.com/en-us/development-tool/pg164100)
- PCU - [(microbit)](https://microbit.org/buy/) (Proprietary)
- RCLS - [dsPIC33CK128MP205](https://www.digikey.ca/en/products/detail/microchip-technology/DSPIC33CK128MP205-I-PT/9342075) (Proprietary)
- RFID Reader - [RC522 RFID MODULE](https://www.digikey.ca/en/products/detail/sunfounder/CN0090/18668629?s=N4IgTCBcDaICxwIyILQGEByAGLBOLKGAIiALoC%2BQA)
- Color Sensor - [APDS-9960 - Light, Ambient Sensor Evaluation Board](https://www.digikey.ca/en/products/detail/adafruit-industries-llc/3595/7652603?s=N4IgTCBcDaIIwFYwA4C0YDMBOBqByAIiALoC%2BQA)
- Speaker - [Passive Buzzer](https://www.mouser.com/ProductDetail/Seeed-Studio/107020109?qs=GedFDFLaBXFNSeAbVfqSzw%3D%3D)
- Microphone - [SPW2430 - MEMS Omnidirectional Microphones Audio Evaluation Board](https://www.digikey.ca/en/products/detail/adafruit-industries-llc/2716/5604375?s=N4IgTCBcDaIIwFYwA4C0cDMA2MqByAIiALoC%2BQA)
- Op-Amp - [Standard (General Purpose) Amplifier 2 Circuit Rail-to-Rail 8-PDIP](https://www.digikey.ca/en/products/detail/texas-instruments/TLV2462IP/277538?s=N4IgTCBcDa4JwDYC0BGAHHAzEgrEgcgCIgC6AvkA)
- Flysky transmitter and receiver

## Communication with PCU via payload requests

Communication is made to PCU via UART protocol.

- **Bytes 1 & 2** - Sync Bytes
- **Bytes 3 & 4** - Type of Message
- **Bytes 5 & 6** - Payload Size

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
SWITCH_A_LSB,
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

# Flysky Controls
<img src="https://github.com/h0nt3d/ECE3232-Rover/blob/main/images/Flysky.png?raw=true">
