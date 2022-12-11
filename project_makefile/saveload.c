#include "header_define.h"
#include "header_function.h"

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