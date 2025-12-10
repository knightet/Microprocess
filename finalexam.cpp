#include "mbed.h"

// === 핀 설정 ===
InterruptIn button(BUTTON1);      // 사용자 버튼 (인터럽트 방식)
DigitalOut trigger(PC_7);         // 초음파 센서 트리거 핀
DigitalIn echo(PB_6);             // 초음파 센서 에코 핀
PwmOut buzzer(PB_0);              // 부저 출력 (PWM 사용)
AnalogIn CdS(PA_0);               // CdS 조도센서 입력 
DigitalOut myled(LED1);           // 디버깅용 LED (내장 LED)
DigitalOut led1(PA_4);            // 조도 상태 LED
DigitalOut led2(PA_5);            // 알람 경고 LED
DigitalOut led3(PA_6);            // 버튼 반응 상태 LED
DigitalIn alarm(D7);              // 외부 알람 입력 신호
Timer timer;                      // 초음파 거리 측정용 타이머

// === 스레드 선언 ===
Thread thread;                   // 초음파 + 부저용 스레드
Thread thread1;                   // 조도센서용 스레드
Thread thread2;                   // 외부 알람 감지용 스레드

// === 함수 프로토타입 ===
void buzzer_1();                  // 거리 감지 및 부저 함수
void light();                     // 조도 감지 및 LED 제어 함수
void move1();                     // 알람 감지 및 경고 LED 함수
void toggle();                    // 버튼 눌림 인터럽트 함수

// === 메인 함수 ===
int main() {
// 각각의 기능을 병렬로 실행하기 위해 스레드 시작
thread.start(buzzer_1);       // 초음파 센서 + 부저
thread1.start(light);         // 조도 센서 + LED
thread2.start(move1);         // 외부 알람 + LED



button.rise(&toggle); // 버튼을 눌렀을 때 toggle() 함수 실행
ThisThread::sleep_for(10s); // 초기화 후 잠시 대기 (시스템 안정화)

while (1) {
    // 메인 스레드는 무한 대기 상태 (모든 기능은 서브 스레드에서 수행)
}



}

// === 버튼 눌렀을 때 상태 LED 토글 ===
void toggle() {
led3 = !led3;  // 버튼 누를 때마다 LED3 켜고/끄기 반복
}

// === 외부 알람 감지 후 LED로 경고 ===
void move1() {
ThisThread::sleep_for(2000ms);  // 초기 안정화 대기


while (1) {
    if (alarm) {           // 알람 신호 감지 시
        led2 = 1;          // LED1 켜기
        ThisThread::sleep_for(500ms);  // 0.5초 유지
        led2 = 0;          // LED2 끄기
        ThisThread::sleep_for(500ms);  // 0.5초 유지
    } else {
        led2 = 0;          // 알람 없으면 끄기
    }
}



}

// === 초음파 거리 측정 및 부저 동작 ===
void buzzer_1() {
float distance = 0.0;
while (1) {
    // 초음파 신호 발생
    trigger = 1;                // 트리거 핀 HIGH 
    myled = 1;                  // 디버깅용 LED 켜기
    timer.reset();              // 타이머 리셋
    wait_us(10);                // 트리거 신호 10μs 유지
    trigger = 0;                // 트리거 핀 low

    // 반사파 대기 및 시간 측정
    while (echo == 0) {} // 초음파 발사후 대기
    timer.start();       // 타이머 시작
    while (echo == 1) {} // 반사파 수신
    timer.stop();        // 타이머 정지

    // 시간 기반 거리 계산 (단위: cm)
    auto round_trip_time = timer.elapsed_time().count();//왕복 시간 측정
    distance = round_trip_time * 0.0173f;//거리계산 공식 시간 x0.0173

    printf("Distance = %f cm \n", distance);//거리값 출력
    ThisThread::sleep_for(100ms); //0.1초대기

    
    if (distance < 10) { // 거리가 10cm 이하이면 부저 울림
        for (int i = 0; i < 5; i++) {
            buzzer.period_us(1000);   // 1kHz 주파수 설정
            buzzer.write(0.5f);       // 소리발생(듀티비 50%)
            ThisThread::sleep_for(100ms); // 0.1초동안유지
        }
        buzzer.write(0.0f);           // 부저 끄기
    }
}



}

// === 조도 센서 값에 따라 LED ON/OFF ===
void light() {
float light = 0.0;//light초기값 설정
while (1) {
    light = CdS.read();               // 아날로그 값 읽기 (0.0 ~ 1.0)
    printf("Light level = %f\n", light);//출력
    ThisThread::sleep_for(100ms); //1초동안 유지

    if (light < 0.4f) {               // 0.4이하의 밝기면
        led1 = 1;                     // LED1 켜기
        ThisThread::sleep_for(200ms); //0.2초동안 유지
    } else {
        led1 = 0;                     // 아니면 LED1 끄기
    }
}
}
