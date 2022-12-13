#include "header_define.h"
#include "header_function.h"

void saveDatas() {
    // 데이터가 얼마 되지 않으므로 init 파일의 내용을 전부 지우고 진행
    int i;
    char s_data[100];
    // a.=========== file 세마포어 =================
    pthread_mutex_lock(&m_savefile);
    pthread_mutex_lock(&m_brightChangeTime);
    pthread_mutex_lock(&m_currentBright);

    setting_fd = fopen(FILE_NAME, "w");

    for (i = 0; i < ARR_SIZE; i++) fprintf(setting_fd, "%d\n", brightChangeTime[i]);
    fprintf(setting_fd, "%d", currentBright);

    fclose(setting_fd);

    sprintf(s_data, "##%02d%02d%02d%02d%03d", brightChangeTime[0], brightChangeTime[1], brightChangeTime[2], brightChangeTime[3], currentBright);
    serialWrite(uart_fd, s_data);
    
    pthread_mutex_unlock(&m_currentBright);
    pthread_mutex_unlock(&m_brightChangeTime);
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