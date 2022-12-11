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

// gcc -o app teamproject.c -lwiringPi -lpthread

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

#define SLAVE_ADDR_01 0x68 

#define BAUD_RATE 115200

#define MAX_HOUR 24
#define MAX_MIN 60

static const char* I2C_DEV = "/dev/i2c-1"; 
static const char* UART2_DEV = "/dev/ttyAMA1";
static const char* FILE_NAME = "config.ini";

// default value : 00:00:00 ~ 06:00:00
int brightChangeTime[4] = { 0, 0, 6, 0}; // 시, 분만 설정
int currentBright;
int currentTime[2];
FILE* setting_fd;
time_t rotery_time1; // 세마포어 필요
int rtc_fd, uart_fd;
int isSettingEnd = 0;
int arg[6] = {0, };
time_t bright;

pthread_mutex_t m_roterySettingDelay, m_currentBright, m_savefile, m_brightChangeTime, m_currentTime;
pthread_mutex_t m_arg, m_isSettingEnd;

#define ARR_SIZE (sizeof(brightChangeTime) / sizeof(int))
#define CONVERT_BCD(x) (((x) / 10) << 4 | ((x) % 10))
#define TIME_TO_INT(h, m) ((h)*100 + (m))
#define IS_IN_BRIGHT_CHANGE_TIME(a, b, c, d, e, f) (TIME_TO_INT(a, b) <= TIME_TO_INT(c, d) && TIME_TO_INT(c, d) <= TIME_TO_INT(e, f))

int clicker = 0;
time_t t;

static char* str[4] = {"sh\0", "sm\0", "eh\0", "em\0"};
unsigned char s = 10, e = 11, h = 12, m = 13;
unsigned char sevenseq[][8] = {
     {0, 0, 0, 0, 0, 0, 1, 1}   //0
    ,{1, 0, 0, 1, 1, 1, 1, 1}   //1
    ,{0, 0, 1, 0, 0, 1, 0, 1}   //2
    ,{0, 0, 0, 0, 1, 1, 0, 1}   //3
    ,{1, 0, 0, 1, 1, 0, 0, 1}   //4
    ,{0, 1, 0, 0, 1, 0, 0, 1}   //5
    ,{0, 1, 0, 0, 0, 0, 0, 1}   //6
    ,{0, 0, 0, 1, 1, 0, 1, 1}   //7
    ,{0, 0, 0, 0, 0, 0, 0, 1}   //8
    ,{0, 0, 0, 0, 1, 0, 0, 1}   //9
    ,{1, 1, 0, 0, 0, 0, 0, 1}   // b, bright.               idx 10
    ,{0, 1, 0, 0, 1, 0, 0, 1}   // S, start                 idx 11
    ,{0, 1, 1, 0, 0, 0, 0, 1}   // E, end                   idx 12
    ,{1, 1, 0, 1, 0, 0, 0, 1}   // h, hour                  idx 13
    ,{1, 1, 0, 1, 0, 1, 0, 1}   // n, minutes font issue    idx 14
};

unsigned char pos[4] = { SEG_4, SEG_3, SEG_2, SEG_1 };
unsigned char digit[10] = {0x7E,0x30,0x6d,0x79,0x33,0x5b,0x5f,0x72,0x7f,0x7b}; // 0~9
unsigned char character[] = {0x5b, 0x4f, 0x1f, 0x17, 0x15}; // S, E, b, h, n

void saveDatas();

int isInBrightChangeTime() {
    if (brightChangeTime[0] > brightChangeTime[2]) {
        return (IS_IN_BRIGHT_CHANGE_TIME(brightChangeTime[0], brightChangeTime[1], currentTime[0], currentTime[1], 24, 0) || IS_IN_BRIGHT_CHANGE_TIME(0, 0, currentTime[0], currentTime[1], brightChangeTime[2], brightChangeTime[3])) ? 1 : 0;
        // return TIME_TO_INT(brightChangeTime[0], brightChangeTime[1]) <= TIME_TO_INT(currentTime[0], currentTime[1]) && TIME_TO_INT(currentTime[0], currentTime[1]) <= TIME_TO_INT(24, 0) && TIME_TO_INT(0, 0) <= TIME_TO_INT(brightChangeTime[2], brightChangeTime[3]) ? 1 : 0;
    }
    else {
        return IS_IN_BRIGHT_CHANGE_TIME(brightChangeTime[0], brightChangeTime[1], currentTime[0], currentTime[1], brightChangeTime[2], brightChangeTime[3]) ? 1 : 0;
        // return TIME_TO_INT(brightChangeTime[0], brightChangeTime[1]) <= TIME_TO_INT(currentTime[0], currentTime[1]) && TIME_TO_INT(currentTime[0], currentTime[1]) <= TIME_TO_INT(brightChangeTime[2], brightChangeTime[3]) ? 1 : 0;
    }
}

int initGpio() {
    if (wiringPiSetupGpio() < 0) {
        return 1;
    }
    pinMode(LED_BRIGHT_COMP_ORI, OUTPUT);
    pinMode(LED_BRIGHT_COMP_MOD, PWM_OUTPUT);
    pinMode(LED_MAIN, PWM_OUTPUT);
    pinMode(ROTERY_CLCK, INPUT);
    pinMode(ROTERY_DATA, INPUT);
    pinMode(ROTERY_CLICK, INPUT);
    pinMode(DS, OUTPUT);
    pinMode(SHCP, OUTPUT);
    pinMode(STCP, OUTPUT);
    pinMode(SEG_4, OUTPUT);
    pinMode(SEG_3, OUTPUT);
    pinMode(SEG_2, OUTPUT);
    pinMode(SEG_1, OUTPUT);
    pullUpDnControl(ROTERY_CLICK, PUD_UP);

    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(PWM_RANGE);
    pwmSetClock(PWM_DIVISOR);
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

int initI2C() {
    if ((rtc_fd = wiringPiI2CSetupInterface(I2C_DEV, SLAVE_ADDR_01)) < 0) {
        return 1;
    }
    return 0;
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

int initUart() {
    if ((uart_fd = serialOpen(UART2_DEV, BAUD_RATE)) < 0) { // UART2 포트 오픈
        return 1;
    }
    return 0;
}

int clickRotary() {
    // read DT status
    int t = digitalRead(ROTERY_CLICK);
    if (t == 0) {
        delay(200);
        return TRUE;
    }
    return -1;
}

void setting(int currentStateCLK) {
    if (clicker == 0) {
        pthread_mutex_lock(&m_currentBright);
        if (digitalRead(ROTERY_DATA) != currentStateCLK) {
            if (currentBright < MAX_BRIGHT) ++currentBright;
        }
        else {
            if (currentBright > MIN_BRIGHT) --currentBright;
        }
        bright = time(NULL);

        // printf("mode:bright bright - %3d, start_time - %3d, %3d end_time - %3d, %3d\n", currentBright, brightChangeTime[0], brightChangeTime[1], brightChangeTime[2], brightChangeTime[3]);
        pthread_mutex_unlock(&m_currentBright);
    }
    else {
        pthread_mutex_lock(&m_brightChangeTime);
        if (digitalRead(ROTERY_DATA) != currentStateCLK)
            ++brightChangeTime[clicker-1];
        else
            --brightChangeTime[clicker-1];

        brightChangeTime[clicker-1] = brightChangeTime[clicker-1] % ((clicker-1)%2 == 0 ? MAX_HOUR : MAX_MIN);
        if (brightChangeTime[clicker-1] < 0) brightChangeTime[clicker-1] += ((clicker-1)%2 == 0 ? MAX_HOUR : MAX_MIN);

        // printf("mode:time%2s bright - %3d, start_time - %3d, %3d end_time - %3d, %3d\n", str[clicker-1], currentBright, brightChangeTime[0], brightChangeTime[1], brightChangeTime[2], brightChangeTime[3]);
        pthread_mutex_unlock(&m_brightChangeTime);
    }
}

void Latch() {
    digitalWrite(STCP, HIGH);
    digitalWrite(STCP, LOW);
}

void FND_out(unsigned char no) {
    shiftOut(DS, SHCP, LSBFIRST, no);
    Latch();
    delay(1);
} 

void clearFnd() {
    for (int i = 0; i < 4; i++) digitalWrite(pos[i], HIGH);
}

void turnOnFnd(unsigned char* data) {
    int i;
    for (i = 0; i < 4; i++) {
        digitalWrite(pos[i], LOW);
        FND_out(data[i]);
        digitalWrite(pos[i], HIGH);
    }
}

void setFndData(int clicker) {
    unsigned char data[4];
    int t = currentBright;
    switch(clicker) {
        case 0:
            data[0] = character[2];
            data[1] = digit[t/100]; t %= 100;
            data[2] = digit[t/10];
            data[3] = digit[t%10];
            break;
        case 1:
            data[0] = character[0];
            data[1] = character[3];
            data[2] = digit[brightChangeTime[0]/10];
            data[3] = digit[brightChangeTime[0]%10];
            break;
        case 2:
            data[0] = character[0];
            data[1] = character[4];
            data[2] = digit[brightChangeTime[1]/10];
            data[3] = digit[brightChangeTime[1]%10];
            break;
        case 3:
            data[0] = character[1];
            data[1] = character[3];
            data[2] = digit[brightChangeTime[2]/10];
            data[3] = digit[brightChangeTime[2]%10];
            break;
        case 4:
            data[0] = character[1];
            data[1] = character[4];
            data[2] = digit[brightChangeTime[3]/10];
            data[3] = digit[brightChangeTime[3]%10];
            break;
        default:
            clearFnd();
            break;
    }

    turnOnFnd(data);
}

void* thRoteryEncoder() { // 5v, clck gpio20, data gpio21
/*     int currentStateCLK;       // CLK의 현재 신호상태 저장용 변수
    int lastStateCLK;          // 직전 CLK의 신호상태 저장용 변수 
    int settingFlag = 0;

    lastStateCLK = digitalRead(ROTERY_CLCK);

    while(1) {
        currentStateCLK = digitalRead(ROTERY_CLCK);

        // TODO: counter 대신 currenBright로 받아와야 함. 세마포어 필요
        // TODO: count 된 후 일정 초가 지나고 데이터 저장 <- 세마포어로 딜레이 되는 시간 최소화
        //      settingDelayTime으로 이름 설정
        if (currentStateCLK != lastStateCLK && currentStateCLK == LOW) { // falling edge
            // a.========= current bright 세마포어 ==============
            pthread_mutex_lock(&m_currentBright);
            if (digitalRead(ROTERY_DATA) != currentStateCLK) {
                if (currentBright < MAX_BRIGHT) ++currentBright;
            }
            else {
                if (currentBright > MIN_BRIGHT) --currentBright;
            }
            printf("current bright : %d\n", currentBright);
            pthread_mutex_unlock(&m_currentBright);
            // a.==============================================
            // b.========= retory time1 세마포어 ==============
            pthread_mutex_lock(&m_roterySettingDelay);
            rotery_time1 = time(NULL); // rotery_time1 == settingDelayTime
            pthread_mutex_unlock(&m_roterySettingDelay);
            // b.============================================
            settingFlag = 1;
        }

        pthread_mutex_lock(&m_roterySettingDelay);
        if ((time(NULL) - rotery_time1) > 3 && settingFlag == 1) {
            settingFlag = 0;
            saveDatas();
        }
        pthread_mutex_unlock(&m_roterySettingDelay);
        

        lastStateCLK = currentStateCLK;
        usleep(1);
    } */
    int currentStateCLK; // CLK의 현재 신호상태 저장용 변수
    int lastStateCLK;    // 직전 CLK의 신호상태 저장용 변수
    int settingFlag = 0;

    lastStateCLK = digitalRead(ROTERY_CLCK);

    while (1)
    {
        currentStateCLK = digitalRead(ROTERY_CLCK);

        if (currentStateCLK != lastStateCLK && currentStateCLK == LOW)
        {
            setting(currentStateCLK);

            // b.========= retory time1 세마포어 ==============
            pthread_mutex_lock(&m_roterySettingDelay);
            rotery_time1 = time(NULL); // rotery_time1 == settingDelayTime
            pthread_mutex_unlock(&m_roterySettingDelay);
            // b.============================================
            settingFlag = 1;
        }

        if (clickRotary() == TRUE)
        {
            clicker++;
            rotery_time1 = time(NULL);
            if (clicker == 5)
            {
                rotery_time1 -= 10;
            }
        }

        if ((time(NULL) - rotery_time1) > 5)
        {
            if (settingFlag == 1) {
                settingFlag = 0;
                clearFnd();
                saveDatas();
            }
            clicker = 0;
        }
        if ((time(NULL) - rotery_time1) <= 5 || settingFlag == 1) {
                pthread_mutex_lock(&m_currentBright);
                pthread_mutex_lock(&m_brightChangeTime);
                setFndData(clicker);
                pthread_mutex_unlock(&m_brightChangeTime);
                pthread_mutex_unlock(&m_currentBright);
        }

        lastStateCLK = currentStateCLK;
        usleep(1);
    }
}

unsigned char serialRead(const int fd) {
    unsigned char x;
    if (read(fd, &x, 1) != 1) // read 함수를 통해 1바이트 읽어옴
        return -1;
    return x; // 읽어온 데이터 반환
}

void serialWrite (const int fd, const unsigned char c) {
    write(fd, &c, 1);
}

void* thBluetooth() {
    int idx_l = 0, idx_a = 0;
    char lines[100] = {'\0', };

    while (1) {
        if (serialDataAvail(uart_fd)) lines[idx_l++] = serialRead(uart_fd);

        else {
            if (strlen(lines) > 0) {
                lines[idx_l-2] = '\0';

                pthread_mutex_lock(&m_arg);
                char* ptr = strtok(lines, " ");
                while (ptr != NULL) {
                    arg[idx_a++] = atoi(ptr);
                    ptr = strtok(NULL, " ");
                }
                pthread_mutex_unlock(&m_arg);
                
                idx_l = 0; idx_a = 0;
                lines[idx_l] = '\0';
                pthread_mutex_lock(&m_isSettingEnd);
                isSettingEnd = 1;
                pthread_mutex_unlock(&m_isSettingEnd);
            }
        }
        delay(10);
    }
}

void* thSettingWithBluetooth() {
    char ret_str[100];
    // 사용자가 정확한 값을 넣는다고 가정. 
    // 실제 앱은 시는 0~23, 분은 0~59까지 설정.
    // 밝기는 10 ~ 100까지의 값으로 설정
    while (1) {
        pthread_mutex_lock(&m_isSettingEnd);
        if (isSettingEnd == 1) {
            pthread_mutex_lock(&m_arg);
            pthread_mutex_lock(&m_currentBright);
            pthread_mutex_lock(&m_brightChangeTime);
            // printf("%d %d %d %d %d %d\n", arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
            // 블루투스 return값이 안옴
            // sprintf(ret_str, "return(origin) - start h:%2d m:%2d, end h:%2d m:%2d, bright:%3d", brightChangeTime[0], brightChangeTime[1], brightChangeTime[2], brightChangeTime[3], currentBright);
            // for (int i = 0; i < strlen(ret_str); i++) serialWrite(uart_fd, ret_str[i]);
            switch(arg[0]) {
                case 1: // bright change start time setting
                    brightChangeTime[0] = arg[1]; brightChangeTime[1] = arg[2];
                    break;
                case 2: // bright change end time setting
                    brightChangeTime[2] = arg[1]; brightChangeTime[3] = arg[2];
                    break;
                case 3: // bright change start time ~ end time setting
                    brightChangeTime[0] = arg[1]; brightChangeTime[1] = arg[2];
                    brightChangeTime[2] = arg[3]; brightChangeTime[3] = arg[4];
                    break;
                case 4: // bright change start time ~ end time and bright setting
                    brightChangeTime[0] = arg[1]; brightChangeTime[1] = arg[2];
                    brightChangeTime[2] = arg[3]; brightChangeTime[3] = arg[4];
                    currentBright = arg[5];
                    break;
                case 5: // bright setting
                    currentBright = arg[1];
                    break;
                default:
                    break;
            }
            // 블루투스 return값이 안옴
            // sprintf(ret_str, "return(modified) - start h:%2d m:%2d, end h:%2d m:%2d, bright:%3d", brightChangeTime[0], brightChangeTime[1], brightChangeTime[2], brightChangeTime[3], currentBright);
            // for (int i = 0; i < strlen(ret_str); i++) serialWrite(uart_fd, ret_str[i]);
            pthread_mutex_unlock(&m_brightChangeTime);
            pthread_mutex_unlock(&m_currentBright);
            pthread_mutex_unlock(&m_arg);
            isSettingEnd = 0;
            saveDatas();
        }
        pthread_mutex_unlock(&m_isSettingEnd);
    }
}

void* thLed() {
    // TODO: 다이얼이 돌아가면 최대 밝기와 현재 밝기가 나타나는 led를 n초동안 켬
    //      다이얼이 돌아가는 동안 초 초기화
    int isBtnOn = LOW, toggle = 0;
    // toggle == 0 -> led off, toggle == 1 -> led on
    while (1) {
        pthread_mutex_lock(&m_roterySettingDelay);
        if ((time(NULL) - rotery_time1) <= 5 && (time(NULL) - bright) <= 5) {
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

void saveDatas() {
    // 데이터가 얼마 되지 않으므로 init 파일의 내용을 전부 지우고 진행
    int i;
    // a.=========== file 세마포어 =================
    pthread_mutex_lock(&m_savefile);
    setting_fd = fopen(FILE_NAME, "w");
    // b.=========== brightChangeTime 세마포어 ===============
    pthread_mutex_lock(&m_brightChangeTime);
    for (i = 0; i < ARR_SIZE; i++) fprintf(setting_fd, "%d\n", brightChangeTime[i]);
    pthread_mutex_unlock(&m_brightChangeTime);
    // b.===================================================
    // c.============ current bright 세마포어 ================
    pthread_mutex_lock(&m_currentBright);
    fprintf(setting_fd, "%d", currentBright);
    pthread_mutex_unlock(&m_currentBright);
    // c.===================================================
    fclose(setting_fd);
    pthread_mutex_unlock(&m_savefile);
    // a.=========================================
}

void loadDatas() {
    int i = 0;
    if (access(FILE_NAME, F_OK) != -1) { // 데이터가 이미 있음. 데이터 불러오기 진행
        setting_fd = fopen(FILE_NAME, "r");
        char buff[CHAR_MAX];
        int temp[ARR_SIZE+1];
        while(!feof(setting_fd)) {
            fgets(buff, CHAR_MAX, setting_fd);
            temp[i++] = atoi(buff);
        }
        for (i = 0; i < ARR_SIZE; i++) brightChangeTime[i] = temp[i];
        currentBright = temp[i];
    }
    else { // 처음 시작했음을 알림. 데이터 초기값 설정
        setting_fd = fopen(FILE_NAME, "w");
        currentBright = MAX_BRIGHT;
        for (i = 0; i < ARR_SIZE; i++) fprintf(setting_fd, "%d\n", brightChangeTime[i]);
        fprintf(setting_fd, "%d\n", currentBright);
        initRTCTime();
    }
    fclose(setting_fd);
}

int main(int argc, char* argv[]) {

    pthread_t threads[5];

    if (initGpio()) return 1;
    if (initI2C()) return 1;
    if (initUart()) return 1;
    initMutex();
    loadDatas();
    initRTCTime(); // 현재 RTC 모듈의 배터리가 없어서 전원이 없으면 

    pthread_create(&threads[0], NULL, thRtc, NULL);
    pthread_create(&threads[1], NULL, thLed, NULL);
    pthread_create(&threads[2], NULL, thRoteryEncoder, NULL);
    pthread_create(&threads[3], NULL, thBluetooth, NULL);
    pthread_create(&threads[4], NULL, thSettingWithBluetooth, NULL);

    for (int i = 0; i < sizeof(threads) / sizeof(pthread_t); i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}