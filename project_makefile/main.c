/*
    금오공과대학교 22학년도 2학기 임베디드 시스템 팀프로젝트

    팀 ARM
    팀장
        20181246 한상현
    팀원
	    20180567 서주원
	    20170523 박희홍

    주제 : 야간에 불을 약간 어둡게 켜는 시스템을 만들어보자
*/

// gcc -o tp teamproject.c -lwiringPi -lpthread

#include "header_define.h"
#include "header_function.h"


int brightChangeTime[4] = { 0, 0, 6, 0}; // 시, 분만 설정
int currentBright;
int currentTime[2];
FILE* setting_fd;
time_t rotery_time1; // 세마포어 필요
int rtc_fd, uart_fd;
int isSettingEnd = 0;
int arg[6] = {0, };
pthread_t threads[THREAD_NUM];

pthread_mutex_t m_roterySettingDelay, m_currentBright, m_savefile, m_brightChangeTime, m_currentTime;
pthread_mutex_t m_arg, m_isSettingEnd;


int main(int argc, char* argv[]) {
    void* functions[THREAD_NUM] = {thRtc, thLed, thRoteryEncoder, thBluetooth, thSettingWithBluetooth};

    if (initGpio()) return 1;
    if (initI2C()) return 1;
    if (initUart()) return 1;
    initMutex();
    loadDatas();
    // 현재 RTC 모듈의 배터리가 없어서 전원이 없으면 초기 시간으로 초기화됨
    // RTC 모듈의 배터리가 있다면 없어도 무방
    initRTCTime();

    signal(SIGINT, sigintHandler);

    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&threads[i], NULL, functions[i], NULL);
    }
    // pthread_create(&threads[0], NULL, thRtc, NULL);
    // pthread_create(&threads[1], NULL, thLed, NULL);
    // pthread_create(&threads[2], NULL, thRoteryEncoder, NULL);
    // pthread_create(&threads[3], NULL, thBluetooth, NULL);
    // pthread_create(&threads[4], NULL, thSettingWithBluetooth, NULL);

    for (int i = 0; i < sizeof(threads) / sizeof(pthread_t); i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}