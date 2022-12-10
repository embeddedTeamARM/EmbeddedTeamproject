#include "header_define.h"

int initGpio() {
    if (wiringPiSetupGpio() < 0) {
        return 1;
    }
    pinMode(LED_BRIGHT_COMP_ORI, OUTPUT);
    pinMode(LED_BRIGHT_COMP_MOD, PWM_OUTPUT);
    pinMode(LED_MAIN, PWM_OUTPUT);
    pinMode(ROTERY_CLCK, INPUT);
    pinMode(ROTERY_DATA, INPUT);
    pinMode(LED_ON_BTN, INPUT);
    pinMode(ROTERY_CLICK, INPUT);
    pullUpDnControl(ROTERY_CLICK, PUD_UP);

    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(PWM_RANGE);
    pwmSetClock(PWM_DIVISOR);
    return 0;
}

int initI2C() {
    if ((rtc_fd = wiringPiI2CSetupInterface(I2C_DEV, SLAVE_ADDR_01)) < 0) {
        return 1;
    }
    return 0;
}

int initUart() {
    if ((uart_fd = serialOpen(UART2_DEV, BAUD_RATE)) < 0) { // UART2 포트 오픈
        return 1;
    }
    return 0;
}

void initMutex() {
    pthread_mutex_init(&m_roterySettingDelay, NULL);
    pthread_mutex_init(&m_currentBright, NULL);
    pthread_mutex_init(&m_savefile, NULL);
    pthread_mutex_init(&m_brightChangeTime, NULL);
    pthread_mutex_init(&m_currentTime, NULL);
    pthread_mutex_init(&m_arg, NULL);
    pthread_mutex_init(&m_isSettingEnd, NULL);
}

void initRTCTime() {
    time_t t = time(NULL);
    struct tm* tInfo = localtime(&t);
    unsigned char sec, min, hour;

    sec = CONVERT_BCD(tInfo->tm_sec);
    min = CONVERT_BCD(tInfo->tm_min);
    hour = CONVERT_BCD(tInfo->tm_hour);

    // printf("%d(%x) %d(%x) %d(%x)\n", sec, sec, min, min, hour, hour);

    wiringPiI2CWriteReg8(rtc_fd, 0x00, sec); // 초
    wiringPiI2CWriteReg8(rtc_fd, 0x01, min); // 분
    wiringPiI2CWriteReg8(rtc_fd, 0x02, hour | 0x00); // 24시간 형식 - 0x00, 12시간 형식 - 0x01
}