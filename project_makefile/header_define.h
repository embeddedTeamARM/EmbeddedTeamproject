#ifndef __TEAMPROJECT_DEFINE__

#define __TEAMPROJECT_DEFINE__

#include <wiringPi.h>
#include <wiringShift.h>
#include <wiringSerial.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define CHAR_MAX 255

#define LED_MAIN            18
#define LED_BRIGHT_COMP_ORI 16
#define LED_BRIGHT_COMP_MOD 19
#define ROTERY_CLCK         20
#define ROTERY_DATA         21
#define ROTERY_CLICK        23
#define STCP                5
#define SHCP                6
#define DS                  7
#define SEG_4               8
#define SEG_3               9
#define SEG_2               10
#define SEG_1               11

#define PWM_RANGE           100
#define PWM_DIVISOR         960

#define MAX_BRIGHT          100
#define MIN_BRIGHT          10
#define MAX_HOUR 24
#define MAX_MIN 60

#define SLAVE_ADDR_01 0x68 

#define BAUD_RATE 115200

static const char* I2C_DEV = "/dev/i2c-1"; 
static const char* UART2_DEV = "/dev/ttyAMA1";
static const char* FILE_NAME = "config.ini";

extern pthread_mutex_t m_roterySettingDelay, m_currentBright, m_savefile, m_brightChangeTime, m_currentTime;
extern pthread_mutex_t m_arg, m_isSettingEnd;

// default value : 00:00 ~ 06:00
extern int brightChangeTime[4]; // 시, 분만 설정
extern int currentBright;
extern int currentTime[2];
extern FILE* setting_fd;
extern time_t rotery_time1; // 세마포어 필요
extern int rtc_fd, uart_fd;
extern int isSettingEnd;
extern int arg[6];
extern time_t bright;

#define ARR_SIZE (sizeof(brightChangeTime) / sizeof(int))
#define CONVERT_BCD(x) (((x) / 10) << 4 | ((x) % 10))
#define TIME_TO_INT(h, m) ((h)*100 + (m))
#define IS_IN_BRIGHT_CHANGE_TIME(a, b, c, d, e, f) (TIME_TO_INT(a, b) <= TIME_TO_INT(c, d) && TIME_TO_INT(c, d) <= TIME_TO_INT(e, f))

#endif