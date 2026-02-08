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
        
        if (RJY > (RJ_Y_CENTER + DEADZONE)) {
            // forward (fixed speed)
            send_motor_settings(1, 100, 1, 100);
        }
        else {
            // stop/brake
            send_motor_settings(0, 0, 0, 0);
        }
    }
}
