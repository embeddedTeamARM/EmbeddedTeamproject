# EmbeddedTeamproject - Team ARM

## 0. 프로젝트 소개



## 목차
[1. 사용 모듈](#1-사용한-모듈)

[2. 사용법](#2-사용법)

[3. 참고자료](#3-참고-자료)

## 1. 사용한 모듈
![i2c_rtc](https://user-images.githubusercontent.com/81803973/206887230-703a8d54-430a-4678-a00d-22256a7fae08.png)
* RTC - I2C   
    + vcc - 3.3v
    + d - GPIO2(I2C data)
    + c - GPUO3(I2C clock)
    + gnd - gnd

- - -

![rotaryencoder](https://user-images.githubusercontent.com/81803973/206887239-9f9d04d1-1494-41a8-a2a2-4edfd9077619.png)
* 로터리 엔코더   
    + vcc - 3.3v
    + gnd - gnd
    + CLK - GPIO20(GPIO.input)
    + DT - GPIO21(GPIO.input)
    + SW - GPIO23(GPIO.input)

- - -

![bluetooth](https://user-images.githubusercontent.com/81803973/206887241-3f8da0dc-67ec-4dc6-91cd-7ecf883745e5.png)
* 블루투스 - UART   
    baud date : 115200bps
    + vcc - 5v
    + gnd - gnd
    + RXD - GPIO0(Raspberry.TXD2)
    + TXD - GPIO1(Raspberry.RXD2)

- - -

* 푸시 버튼   
    슬라이드 스위치가 없어 푸시 버튼으로 대체   
    버튼을 누르고 있는 중에만 켜지도록 함
    
- - -

* LED
    + Main LED - GPIO18(GPIO.PWM_OUTPUT)
    + Bright Setting LED
        - 최대 밝기 인디케이터 - GPIO16(GPIO.output)
        - 설정 밝기 인디케이터 - GPIO19(GPIO.PWM_OUTPUT)

## 2. 사용법
1. 데이터 설정
    * 1.1 - 블루투스로 설정   
        + 블루투스는 다음과 같은 형태로 데이터를 받는다.   
            <pre>[code] [digit1] [digit2] [digit3] [digit4] [digit5]</pre>
            |code|digit1|digit2|digit3|digit4|digit5|설명|
            |-----|---|---|---|---|---|---|
            |1|0~23|0~59|-|-|-|시작시간 설정|
            |2|0~23|0~59|-|-|-|끝 시간 설정|
            |3|0~23|0~59|0~23|0~59|-|시작시간과 끝 시간 설정|
            |4|0~23|0~59|0~23|0~59|10~100|시작시간, 끝 시간, 밝기 설정|
            |5|10~100|-|-|-|-|밝기 설정|
            
    * 1.2 - 다이얼로 설정
        + 다이얼로 설정하는 방식은 로터리 버튼을 클릭하여 모드를 바꿔 설정한다.   
            <pre>  case 1: 기본 조절값
                밝기 조절
            case 2: 다이얼 1번 클릭
                시작 시각 중 시각 설정
            case 3: 다이얼 2번 클릭
                시작 시각 중 분 설정
            case 4: 다이얼 3번 클릭
                끝 시각 중 시각 설정
            case 5: 다이얼 4번 클릭
                끝 시각 중 분 설정</pre>   
           case 2~5일 경우, 시각 조절이 n초동안 일어나지 않는다면 default로 돌아간다.
2. 
3. 
4. 

## 3. 참고 자료
* [74HC595 datasheet](https://www.sparkfun.com/datasheets/IC/SN74HC595.pdf)
* [4 digit 7 segment with 74HC595](https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=pcmola&logNo=220610851580)
* [rotary encoder click](https://learn.sunfounder.com/lesson-18-rotary-encoder/)
* 금오공대 임베디드 시스템 강의 자료
* [마크다운 표 만들기](https://github.com/inasie/inasie.github.io/blob/master/_posts/2018-11-25-%EB%A7%88%ED%81%AC%EB%8B%A4%EC%9A%B4-%ED%91%9C-%EB%A7%8C%EB%93%A4%EA%B8%B0.md)
* [마크다운 만들기](https://gist.github.com/ihoneymon/652be052a0727ad59601)
* [마크다운 목차 만들기](https://kyeoneee.tistory.com/56)