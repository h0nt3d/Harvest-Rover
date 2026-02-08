#include "config.h"
#include "init.h"
#include <xc.h>

void main() 
{    
    initialize();
    while (1) {
        rxCount = 0;
        rxDone  = 0;

        send_get_flySky_info();
        while (!rxDone) {}
        
        uint8_t rjy_lsb = get_flySky_info_buf[8];
        uint8_t rjy_msb = get_flySky_info_buf[9];
        
        uint16_t RJY = (uint16_t)rjy_lsb | ((uint16_t)rjy_msb << 8);
        int16_t offset = (int16_t)RJY - (int16_t)RJ_Y_CENTER;
        
        uint8_t pwm = 0;
        
        // Forward
        if (offset > DEADZONE) {
            offset -= DEADZONE;
            
            if (offset > MAX_FWD_OFFSET)
                offset = MAX_FWD_OFFSET;
            
            pwm = (uint8_t)((offset * 100) / MAX_FWD_OFFSET);
            
            send_motor_settings(1, pwm, 1, pwm);
        }
        
        // Backward
        else if (offset < -DEADZONE) {
            offset = -offset;
            offset += DEADZONE;
            
            if (offset > MAX_FWD_OFFSET)
                offset = MAX_FWD_OFFSET;
            
            pwm = (uint8_t)((offset * 100) / MAX_FWD_OFFSET);
            
            send_motor_settings(2, pwm, 2, pwm);
        }
        
        // Stop
        else {
            send_motor_settings(0, 0, 0, 0);
        }
        
        
    }
}
