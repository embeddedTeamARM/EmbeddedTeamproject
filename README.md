# EmbeddedTeamproject - Team ARM

## 사용한 모듈
* RTC - I2C
    + vcc - 3.3v
    + d - GPIO2(I2C data)
    + c - GPUO3(I2C clock)
    + gnd - gnd
    
* 로터리 엔코더
    + vcc - 3.3v
    + gnd - gnd
    + CLK - GPIO20(GPIO.input)
    + DT - GPIO21(GPIO.input)
    + SW - GPIO23(GPIO.input)

* 블루투스 - UART   
    baud date : 115200bps
    + vcc - 5v
    + gnd - gnd
    + RXD - GPIO0(Raspberry.TXD2)
    + TXD - GPIO1(Raspberry.RXD2)

* 버튼   
    슬라이드 스위치가 없어 버튼으로 대체   
    버튼을 누르고 있는 중에만 켜지도록 함

* LED
    + Main LED - GPIO18(GPIO.PWM_OUTPUT)
    + Bright Setting LED
        - 최대 밝기 인디케이터 - GPIO16(GPIO.output)
        - 설정 밝기 인디케이터 - GPIO19(GPIO.PWM_OUTPUT)