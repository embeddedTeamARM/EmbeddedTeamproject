#include "header_define.h"
#include "header_function.h"

int clicker = 0;
time_t bright;
static char* str[4] = {"sh\0", "sm\0", "eh\0", "em\0"};

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
            settingFlag = 1;
            rotery_time1 = time(NULL);
            if (clicker == 5)
            {
                clicker = 0;
                rotery_time1 -= 10;
            }
            printf("%d\n", clicker);
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