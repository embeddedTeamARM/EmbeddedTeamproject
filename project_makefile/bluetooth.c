#include "header_define.h"
#include "header_function.h"

unsigned char serialRead(const int fd) {
    unsigned char x;
    if (read(fd, &x, 1) != 1) // read 함수를 통해 1바이트 읽어옴
        return -1;
    return x; // 읽어온 데이터 반환
}

void serialWrite(const int fd, const unsigned char c) {
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