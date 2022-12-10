#include "header_define.h"

int isInBrightChangeTime() {
    if (brightChangeTime[0] > brightChangeTime[2]) {
        return (IS_IN_BRIGHT_CHANGE_TIME(brightChangeTime[0], brightChangeTime[1], currentTime[0], currentTime[1], 24, 0) || IS_IN_BRIGHT_CHANGE_TIME(0, 0, currentTime[0], currentTime[1], brightChangeTime[2], brightChangeTime[3])) ? 1 : 0;
        return TIME_TO_INT(brightChangeTime[0], brightChangeTime[1]) <= TIME_TO_INT(currentTime[0], currentTime[1]) && TIME_TO_INT(currentTime[0], currentTime[1]) <= TIME_TO_INT(24, 0) && TIME_TO_INT(0, 0) <= TIME_TO_INT(brightChangeTime[2], brightChangeTime[3]) ? 1 : 0;
    }
    else {
        return IS_IN_BRIGHT_CHANGE_TIME(brightChangeTime[0], brightChangeTime[1], currentTime[0], currentTime[1], brightChangeTime[2], brightChangeTime[3]) ? 1 : 0;
        return TIME_TO_INT(brightChangeTime[0], brightChangeTime[1]) <= TIME_TO_INT(currentTime[0], currentTime[1]) && TIME_TO_INT(currentTime[0], currentTime[1]) <= TIME_TO_INT(brightChangeTime[2], brightChangeTime[3]) ? 1 : 0;
    }
}

void* thLed() {
    // TODO: 다이얼이 돌아가면 최대 밝기와 현재 밝기가 나타나는 led를 n초동안 켬
    //      다이얼이 돌아가는 동안 초 초기화
    int isBtnOn = LOW, toggle = 0;
    // toggle == 0 -> led off, toggle == 1 -> led on
    while (1) {
        pthread_mutex_lock(&m_roterySettingDelay);
        if ((time(NULL) - rotery_time1) <= 5) {
            digitalWrite(LED_BRIGHT_COMP_ORI, HIGH);
            pwmWrite(LED_BRIGHT_COMP_MOD, currentBright);
        }
        else {
            digitalWrite(LED_BRIGHT_COMP_ORI, LOW);
            pwmWrite(LED_BRIGHT_COMP_MOD, 0);
        }
        pthread_mutex_unlock(&m_roterySettingDelay);

        pthread_mutex_lock(&m_currentTime);
        // IsInBrightChangeTime(brightChangeTime, currentTime);
        pthread_mutex_lock(&m_brightChangeTime);
        if (isInBrightChangeTime() == 1) {
            pwmWrite(LED_MAIN, currentBright);
        }
        else {
            pwmWrite(LED_MAIN, MAX_BRIGHT);
        }
        pthread_mutex_unlock(&m_brightChangeTime);
        pthread_mutex_unlock(&m_currentTime);
        delay(100);
    }
}