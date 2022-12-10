#include "header_define.h"
#include "header_function.h"

void* thRoteryEncoder() { // 5v, clck gpio20, data gpio21
    int currentStateCLK;       // CLK의 현재 신호상태 저장용 변수
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
    }
}