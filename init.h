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



#endif