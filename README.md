# ECE3232-Rover

## Hardware:
- MPLAB Xpress Development Board (PIC16F18855)
- Snap Programmer
- PCU - UNB Dev Board (microbit)
- RCLS - Radio Controlled Laser System

## 3D Components:
- M3 Size Screws and Washers
- M3 Size Female to female standoff


## Communication with PCU via payload requests

### **Get PCU Info Command**  <br>
```c
volatile uint8_t get_pcu_info[6] = {0xFE, 0x19, 0x01, 0x04, 0x00, 0x00};
```

**PCU Info Respsone** <br>
12 bit buffer <br>
`[SYNC1, SYNC2, IDLSB, IDMSB, SIZELSB, SIZEMSB, TEAMID, PLAYERID, HEALTHLSB, HEALTHMSB, SHIELDFLAG, REPAIRFLAG]`


# Flysky Controls
<img src="https://github.com/h0nt3d/ECE3232-Rover/blob/main/images/Flysky.png?raw=true">
