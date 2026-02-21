#include "config.h"
#include "init.h"
#include <xc.h>

void main() 
{    
    initialize();
    send_set_laser_scope();
    send_shoot_laser();
    while (1) {
        rxCount = 0;
        rxDone  = 0;

        send_get_flySky_info();
        while (!rxDone) {}
        
        uint8_t rjy_lsb = get_flySky_info_buf[8];
        uint8_t rjy_msb = get_flySky_info_buf[9];
        
        uint8_t ljy_lsb = get_flySky_info_buf[12];
        uint8_t ljy_msb = get_flySky_info_buf[13];
        
        uint16_t RJY = (uint16_t)rjy_lsb | ((uint16_t)rjy_msb << 8);
        uint16_t LJY = (uint16_t)ljy_lsb | ((uint16_t)ljy_msb << 8);
        
        int16_t offset_rj = (int16_t)RJY - (int16_t)RJ_Y_CENTER;
        int16_t offset_lj = (int16_t)LJY - (int16_t)LJ_Y_CENTER;
        
        uint8_t pwm = 0;
        
        // Forward
        if (offset_rj > DEADZONE) {
            offset_rj -= DEADZONE;
            
            if (offset_rj > MAX_FWD_OFFSET)
                offset_rj = MAX_FWD_OFFSET;
            
            pwm = (uint8_t)((offset_rj * 100) / MAX_FWD_OFFSET);
            
            send_motor_settings(1, pwm, 1, pwm);
        }
        
        // Backward
        else if (offset_rj < -DEADZONE) {
            offset_rj = -offset_rj;
            offset_rj += DEADZONE;
            
            if (offset_rj > MAX_FWD_OFFSET)
                offset_rj = MAX_FWD_OFFSET;
            
            pwm = (uint8_t)((offset_rj * 100) / MAX_FWD_OFFSET);
            
            send_motor_settings(2, pwm, 2, pwm);
        }
        
        //Right
        else if (offset_lj > DEADZONE) {
            offset_lj -= DEADZONE;
            
            if (offset_lj > MAX_FWD_OFFSET)
                offset_lj = MAX_FWD_OFFSET;
            
            pwm = (uint8_t)((offset_lj * 100) / MAX_FWD_OFFSET);
            
            send_motor_settings(1, pwm, 2, pwm);
        }
        
        else if (offset_lj < -DEADZONE) {
            /*offset_lj -= DEADZONE;*/
            offset_lj = -offset_lj;
            offset_lj += DEADZONE;
            
            
            if (offset_lj > MAX_FWD_OFFSET)
                offset_lj = MAX_FWD_OFFSET;
            
            pwm = (uint8_t)((offset_lj * 100) / MAX_FWD_OFFSET);
            
            send_motor_settings(2, pwm, 1, pwm);
        }
        
        // Stop
        else {
            send_motor_settings(0, 0, 0, 0);
        }
        
        
    }
}
