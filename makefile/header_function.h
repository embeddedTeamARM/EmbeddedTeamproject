#ifndef __TEAMPROJECT_FUNCTION__

#define __TEAMPROJECT_FUNCTION__

int isInBrightChangeTime();

int initGpio();
int initI2C();
int initUart();
void initMutex();
void initRTCTime();

void* thRoteryEncoder();

unsigned char serialRead(const int fd);
void serialWrite(const int fd, const unsigned char c);
void* thBluetooth();

void* thSettingWithBluetooth();

void* thLed();

void* thRtc();

void loadDatas();
void saveDatas();

#endif