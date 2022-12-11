#include "header_define.h"

unsigned char pos[4] = { SEG_4, SEG_3, SEG_2, SEG_1 };
unsigned char digit[10] = {0x7E,0x30,0x6d,0x79,0x33,0x5b,0x5f,0x72,0x7f,0x7b}; // 0~9
unsigned char character[] = {0x5b, 0x4f, 0x1f, 0x17, 0x15}; // S, E, b, h, n

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