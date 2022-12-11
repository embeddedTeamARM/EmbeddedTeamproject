#include "header_define.h"

void* thRtc() {
    unsigned char timeSheet[] = {0x01, 0x02};
    int temp, t;
    int timesheetSize = sizeof(timeSheet) / sizeof(unsigned char);
    
    while(1) {
        // ================ current time세마포어 ===============
        pthread_mutex_lock(&m_currentTime);
        for (int i = 0; i < timesheetSize; i++) {
            temp = wiringPiI2CReadReg8(rtc_fd, timeSheet[i]);
            // currentTime[i] = i < 2 ? temp & 0x7F : temp & 0x3F;
            switch (i) {
                case 0: // 분 
                    t = temp & 0x7F;
                    break;
                case 1: // 시
                    t = temp & 0x3F;
                    break;
            }
            currentTime[timesheetSize-1-i] = (t & 0x0F) + (((t >> 4) & 0x0F)*10);
        }
        pthread_mutex_unlock(&m_currentTime);
        delay(1000);
    }
}