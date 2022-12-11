# EmbeddedTeamproject - Team ARM

## 사용한 모듈
![i2c_rtc](https://user-images.githubusercontent.com/81803973/206887230-703a8d54-430a-4678-a00d-22256a7fae08.png)
* RTC - I2C   
    + vcc - 3.3v
    + d - GPIO2(I2C data)
    + c - GPUO3(I2C clock)
    + gnd - gnd
    
![rotaryencoder](https://user-images.githubusercontent.com/81803973/206887239-9f9d04d1-1494-41a8-a2a2-4edfd9077619.png)
* 로터리 엔코더   
    + vcc - 3.3v
    + gnd - gnd
    + CLK - GPIO20(GPIO.input)
    + DT - GPIO21(GPIO.input)
    + SW - GPIO23(GPIO.input)

![bluetooth](https://user-images.githubusercontent.com/81803973/206887241-3f8da0dc-67ec-4dc6-91cd-7ecf883745e5.png)
* 블루투스 - UART   
    baud date : 115200bps
    + vcc - 5v
    + gnd - gnd
    + RXD - GPIO0(Raspberry.TXD2)
    + TXD - GPIO1(Raspberry.RXD2)

* 푸시 버튼   
    슬라이드 스위치가 없어 푸시 버튼으로 대체   
    버튼을 누르고 있는 중에만 켜지도록 함

* LED
    + Main LED - GPIO18(GPIO.PWM_OUTPUT)
    + Bright Setting LED
        - 최대 밝기 인디케이터 - GPIO16(GPIO.output)
        - 설정 밝기 인디케이터 - GPIO19(GPIO.PWM_OUTPUT)

## 참고 자료 출처
* [74HC595 datasheet](https://www.sparkfun.com/datasheets/IC/SN74HC595.pdf)
* [4 digit 7 segment with 74HC595](https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=pcmola&logNo=220610851580)
* [rotary encoder click](https://learn.sunfounder.com/lesson-18-rotary-encoder/)
* 금오공대 임베디드 시스템 강의 자료