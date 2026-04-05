#ifndef INIT_H
#define INIT_H

#include <stdint.h>

#define DEADZONE 40
#define RJ_Y_CENTER 0x05DD
#define LJ_Y_CENTER 0x05DD
#define MAX_FWD_OFFSET 500


extern volatile uint8_t rxCount;
extern volatile uint8_t rxDone;
extern volatile uint8_t get_flySky_info_buf[26];

void initialize();
void send_get_flySky_info();
void send_motor_settings(uint8_t dirA, uint8_t pwmA, uint8_t dirB, uint8_t pwmB);
void send_set_laser_scope();
void send_shoot_laser();

void I2C_Init();
void APDS9960_Init();
void APDS9960_Reset();


typedef struct {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t clear;
} RGBC_t;
RGBC_t APDS9960_ReadColors();


extern uint8_t uid[4];
void RFID_initialize();
void RC522_InitChip();
uint8_t RC522_ReadUID(uint8_t *uid4);


#endif