#include "header_define.h"
#include "header_function.h"

void clearGpio() {
    pwmWrite(LED_MAIN, LOW);
    pwmWrite(LED_BRIGHT_COMP_MOD, LOW);
    digitalWrite(LED_BRIGHT_COMP_ORI, LOW);
    digitalWrite(DS, LOW);
    digitalWrite(SHCP, LOW);
    digitalWrite(STCP, LOW);
}

void sigintHandler(int sig) {
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_cancel(threads[i]);
    }
    
    saveDatas();
    
    clearFnd();
    clearGpio();

    close(rtc_fd);
    close(uart_fd);

    exit(0);
}