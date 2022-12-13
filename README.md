# EmbeddedTeamproject - Team ARM

<details>
<summary>목차</summary>

0. [프로젝트 소개](#0-프로젝트-소개)
0. [개발 내용](#1-개발-내용)
0. [사용 모듈](#2-사용-모듈)
0. [ 사용법](#3-사용법)
0. [ 한계](#4-한계)
0. [참고자료](#5-참고-자료)

</details>

- - -

## 0. 프로젝트 소개 및 개요   

* 프로젝트 소개

    임베디드 시스템 팀 프로젝트의 주제가 `실생활에 활용가능한 임베디드 시스템을 제작해보자`이다. 

    제작하는 시스템의 안건으로 간이 무드등, 타이머, 충돌방지 모듈, 야간 화장실 등 밝기 조절이 있었다. 이 중 야간 화장실 등 밝기 조절의 아이디어가 좋아 해당 주제로 제작하게 되었다.

    실생활에서 야간에 동거인이 화장실, 거실, 주방 등의 불을 켜서 잠에서 꺠는 경험이 종종 있을 것이다. 빛으로 인해 수면에서 자주 깰 경우에는 사람의 일주기성 리듬이 교란되기 쉬우며, 이로인해 수면의 질이 떨어지고 심할경우에는 비만, 당뇨병 등과 같은 건강문제까지 이어질 수 있다. 우리는 이러한 불편함을 해소하고자 야간에 특정 조명의 밝기를 조절할 수 있는 시스템을 개발하고자 한다.
        
    사용자는 블루투스로 예약시간 및 조명 밝기를 설정하여, 해당 시간동안은 설정한 밝기로 조명을 조절할 수 있다. 또한 실시간으로 조절하길 원하는 경우, 수동으로 원하는 밝기를 조절할 수 있다. 예약 시간 종료 후에는 조명이 다시 원래의 최대 밝기로 돌아오게 된다.

    배포한 스마트폰 앱을 통해 밝기를 조절하는 시간대와 밝기를 조절할 수 있는 제공할 수 있도록 한다.

* 프로젝트 개요

    1. 제한 요소

        `쓰레드/Mutex` 혹은 `멀티프로세싱/IPC` 를 필수로 사용한다.

    2. 전체 시스템 구조


    3. 제한 요소 구현   

        + 각 함수들은 pthread_create 함수를 이용하여 스레드로 동작하도록 한다. 
            ```c
            for (int i = 0; i < THREAD_NUM; i++) {
                pthread_create(&threads[i], NULL, functions[i], NULL);
            }

            for (int i = 0; i < sizeof(threads) / sizeof(pthread_t); i++) {
                pthread_join(threads[i], NULL);
            }
            ```
        + 밝기 설정 및 밝기 조절 시간은 아래와 같이 mutex를 사용하여 상호 간섭이 일어나지 않게 한다.
            ```c
            if (clicker == 0) {
                pthread_mutex_lock(&m_currentBright);
                if (digitalRead(ROTERY_DATA) != currentStateCLK) {
                    if (currentBright < MAX_BRIGHT) currentBright += 5;
                }
                else {
                    if (currentBright > MIN_BRIGHT) currentBright -= 5;
                }
                pthread_mutex_unlock(&m_currentBright);
            }
            else {
                pthread_mutex_lock(&m_brightChangeTime);
                if (digitalRead(ROTERY_DATA) != currentStateCLK)
                    ++brightChangeTime[clicker-1];
                else
                    --brightChangeTime[clicker-1];
                pthread_mutex_unlock(&m_brightChangeTime);
            }
            ```
        + 프로그램에 SIGINT(ctrl+c) 인터럽트가 발생할 경우, 모든 스레드를 강제중단시키고 GPIO_OUTPUT으로 설정한 핀의 값을 LOW 값으로 설정한다.
            ```c
            void sigintHandler(int sig) {
                for (int i = 0; i < THREAD_NUM; i++) {
                    pthread_cancel(threads[i]); // 스레드 종료
                }
                
                clearFnd(); // 모든 fnd를 끔
                clearGpio(); // GPIO_OUTPUT으로 설정한 핀의 값을 LOW 값으로 설정

                close(rtc_fd); // rtc 디바이스 끄기
                close(uart_fd); // uart(bluetooth) 디바이스 끄기

                exit(0); // 프로그램 종료
            }

            signal(SIGINT, sigintHandler); // main function
            ```

- - -

## 1. 개발 내용

※ 이하 내용 중 `설정한 시간`은 `야간`으로 대체한다.

1) 중간 목표

    * 로터리 엔코더를 이용하여 1) led에 전원이 들어가고 2) 야간이 되면 led의 밝기가 설정한 값으로 변하도록 한다.
    * 로터리 엔코더를 이용하여 야간 led 밝기를 조절하면 밝기가 어느정도 되는지 알려주는 인디케이터를 배치한다.
    * 블루투스를 이용하여 야간에 켜지는 led 밝기를 조절할 수 있게 한다.

2) 최종 목표  
    중간목표를 포함하여 아래의 항목들을 구현한다.
    * 중간 목표의 블루투스 항목을 확대하여 제어하는 전용 스마트폰 앱을 만들어 야간 시각, 끝 시각, 야간 밝기를 설정할 수 있게 한다.
    * 로터리 엔코더의 클릭 기능을 활용하여 앱 없이도 야간 시작 시각, 끝 시각을 설정할 수 있게 한다.
    * 4 digit FND를 이용하여 현재 설정하려는 값이 어떤 값인지 알려주도록 한다.
- - -

## 2. 사용 모듈
* Base

    ![raspberry](https://user-images.githubusercontent.com/81803973/207356044-52c6c40e-fef7-4eb7-9811-61906ef4e110.png)
    - 라즈베리파이 4 model B
    
* Module

    ![i2c_rtc](https://user-images.githubusercontent.com/81803973/206887230-703a8d54-430a-4678-a00d-22256a7fae08.png)
    - RTC - I2C   
        + vcc - 3.3v
        + d - GPIO2(I2C data)
        + c - GPUO3(I2C clock)
        + gnd - gnd


    ![rotaryencoder](https://user-images.githubusercontent.com/81803973/206887239-9f9d04d1-1494-41a8-a2a2-4edfd9077619.png)
    - 로터리 엔코더   
        + vcc - 3.3v
        + gnd - gnd
        + CLK - GPIO20(GPIO.input)
        + DT - GPIO21(GPIO.input)
        + SW - GPIO23(GPIO.input)


    ![bluetooth](https://user-images.githubusercontent.com/81803973/206887241-3f8da0dc-67ec-4dc6-91cd-7ecf883745e5.png)
    - 블루투스 - UART   
        baud date : 115200bps
        + vcc - 5v
        + gnd - gnd
        + RXD - GPIO0(Raspberry.TXD2)
        + TXD - GPIO1(Raspberry.RXD2)


    ### 4digit 7세그먼트 관리

    ![4_digit_7_segment](https://user-images.githubusercontent.com/81803973/206914131-30608036-62bb-400b-887c-7fdbec62ef91.png)
    - 7 Segment : LOW일때 해당 FND에 값 출력
        + digit4(D1) - GPIO8(GPIO.output)
        + digit3(D2) - GPIO9(GPIO.output)
        + digit2(D3) - GPIO10(GPIO.output)
        + digit1(D4) - GPIO11(GPIO.output)

    ![74hc595](https://user-images.githubusercontent.com/81803973/206914137-ec4febc1-3798-4df4-9778-46a995292ce8.png)
    - 74hc595
        + STCP(RCLK, 12) - GPIO5(시프트 레지스터가 수신한 데이터를 래치 레지스터에 저장하도록 해주는 신호선, GPIO.output)
        + SHCP(SRCLK, 11) - GPIO6(라즈베리파이와 시프트 레지스터 간 클럭 동기화, GPIO.output)
        + DS(SER, 14) - GPIO7(ic칩으로 데이터 전송, GPIO.output)
        

    현재 7segment에서 decimal(점 표현)은 필요 없음. 따라서 Qb ~ Qh까지 쓰는것으로 회로를 단순화   
    Qb는 segment-A, Qc는 segment-B, ... , Qh는 segment-G에 연결하여 세그먼트에 숫자를 출력

    ![slideswitch](https://user-images.githubusercontent.com/81803973/207092003-77199bff-a790-42b5-bb72-512c93281597.jpg)
    - 슬라이드 스위치   
    슬라이드 스위치를 좌로 움직이는지 우로 움직이는지에 따라 출력 값을 변경.

    ![led](https://user-images.githubusercontent.com/81803973/207091991-a64c6906-e28e-4f55-9a76-ef838ff00a11.jpg)
    - LED
        + Main LED - GPIO18(GPIO.PWM_OUTPUT)
        + Bright Setting LED
            - 최대 밝기 인디케이터 - GPIO16(GPIO.output)
            - 설정 밝기 인디케이터 - GPIO19(GPIO.PWM_OUTPUT)

- - -

## 3. 사용법
0) 프로그램 컴파일 및 실행
    1. project_makefile 디렉터리로 이동한다.
    2. make 명령어를 입력한다.
    3. 생성된 실행 파일은 app일 것이다. ```sudo ./app``` 으로 실행한다.   
    

1) 데이터 설정
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
           case 2~5일 경우, 시각 조절이 n초동안 일어나지 않는다면 case1로 돌아간다.

           다이얼로 설정할 경우 7segment에 설정하려는 값이 나타난다.
2) 7-segment   
    * 표현 식   
        |code|설명|숫자 자릿수|
        |-----|---|---:|
        |Sh|시작 시각 indicater|2|
        |Sn|시작 분 indicater|2|
        |Eh|끝 시각 indicater|2|
        |En|끝 분 indicater|2|
        |b|설정 밝기 indicater|3|

    로터리 엔코더의 값이 변경됨에 따라 FND의 LED가 점등된다.

3) 슬라이드 스위치   
    a. 슬라이드 스위치를 왼쪽으로 옮기면 불이 꺼진다.   
    b. 슬라이드 스위치를 오른쪽으로 옮기면 불이 켜진다.

4) 시스템에서 사용자 휴대폰으로 데이터를 전송   
    1~5 이외의 값을 1.1에서 code 값으로 넘기면 데이터를 수신하는 모드로 설정된다.
    휴대폰에서 수신되는 값의 형식은 다음과 같다.

    <pre>##aabbccddeee</pre>

    * ```#``` : magic number. 가끔 휴대전화에서 데이터 수신 시 일부 데이터가 누락되는 문제 발생
    * aa : 시작 시각
    * bb : 시작 분
    * cc : 끝 시각
    * dd : 끝 분
    * eee : 현재 밝기

    휴대전화 앱에서 데이터를 수신하면 수신된 값을 parsing하여 사용자에게 보여주도록 한다.
- - -

## 4. 한계 및 해결 방안
1. 아직 블루투스만을 지원한다. 모듈을 추가하고 데이터를 받는 부분을 wifi로도 받을 수 있게 한다면, wifi 통신도 가능할 것으로 생각한다.

2. led의 밝기가 조절되는 것이 부자연스럽다. 현재 시각이 조명의 밝기를 어둡게 혹은 밝게 하는 시간대에 있다면(예를 들어 현재 밝기가 100이고 설정한 밝기가 20이라면 100에서 20까지 단계적으로 밝기가 줄어드는 것이 좋을것이다) 연속된 밝기로 조절이 될 것이라 생각하였다. 하지만 실제 구현해보니 mutex 등 여러 조건으로 인해 생각한 조절 방식이 구현되지 않았다.

3. 라즈베리파이의 한계 상 현재 세팅값들은 ROM에 저장되는 것이 아닌 sd카드에 저장된다. 추후에 ROM 모듈이 생긴다면 ROM 모듈에 데이터를 저장할 수 있을 것이다.

4. RTC 모듈의 배터리가 부족하여 라즈베리파이의 전원을 껐다 켤 때마다 RTC 내부의 값이 초기화 되었다. 현재 라즈베리파이는 wifi 연결이 되므로 현재 시스템 시각을 이용하여 RTC 값을 초기화 한 후 RTC 모듈 내부의 값을 읽어 현재 시각을 파악하도록 하였다.

5. 스마트폰 앱에서 블루투스 모듈로 데이터를 전송하는 것은 문제가 없었지만, 블루투스 모듈에서 스마트폰 앱으로 현재 세팅 데이터를 전송하는 것에서 일부 데이터가 누락되는 상황이 발생하였다. 

6. mutex를 자주 사용하다보니 데이터를 출력하는데 딜레이가 자주 발생하였다. 밝기 및 시각은 변경까지 시간이 조금 걸려도 상관 없으니 단순히 FND 혹은 LED에 값을 출력하는 경우에는 mutex를 사용하지 않도록 하였다.

- - -

## 5. 참고 자료
* 모듈 관련
    - [74HC595 datasheet](https://www.sparkfun.com/datasheets/IC/SN74HC595.pdf)
    - [4 digit 7 segment with 74HC595](https://blog.naver.com/pcmola/220610851580)
    - [4 digit 7 segment image](http://www.learningaboutelectronics.com/Articles/4-digit-7-segment-LED-circuit-with-an-arduino.php)
    - [74HC595 image](https://blog.naver.com/dmaker123/221894002813)
    - [rotary encoder click](https://learn.sunfounder.com/lesson-18-rotary-encoder/)
    - 금오공대 임베디드 시스템 강의 자료
* git 관련
    - [마크다운 표 만들기](https://github.com/inasie/inasie.github.io/blob/master/_posts/2018-11-25-%EB%A7%88%ED%81%AC%EB%8B%A4%EC%9A%B4-%ED%91%9C-%EB%A7%8C%EB%93%A4%EA%B8%B0.md)
    - [마크다운 만들기](https://gist.github.com/ihoneymon/652be052a0727ad59601)
    - [마크다운 목차 만들기](https://kyeoneee.tistory.com/56)
    - [git push시 403 에러](https://devsurimlee.tistory.com/33)
