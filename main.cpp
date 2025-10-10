// @file      main.cpp
// @author    Donghee Park
//
// Copyright (c) 2024 Donghee Park, all rights reserved

#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <ctime>
#include <cstdio>

// 화면 크기 설정 (200x200으로 축소)
const int SCREEN_WIDTH = 200;
const int SCREEN_HEIGHT = 200;

// 시계 설정 (작은 화면에 맞게 조정)
const int CLOCK_RADIUS = 80;
const Vector2 CLOCK_CENTER = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};

// 시계 구조체
struct AnalogClock {
    Vector2 center;
    float radius;
    float hourHandLength;
    float minuteHandLength;
    float secondHandLength;
    float hourAngle;
    float minuteAngle;
    float secondAngle;
    int hours;
    int minutes;
    int seconds;
};

// 현재 시간을 가져오는 함수
void UpdateTime(AnalogClock* clock) {
    time_t rawTime;
    struct tm* timeInfo;

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    clock->hours = timeInfo->tm_hour % 12;
    clock->minutes = timeInfo->tm_min;
    clock->seconds = timeInfo->tm_sec;

    // 각도 계산 (12시 방향을 0도로 하고 시계방향으로 증가)
    clock->secondAngle = (clock->seconds * 6.0f) - 90.0f;  // 6도씩 증가
    clock->minuteAngle = (clock->minutes * 6.0f + clock->seconds * 0.1f) - 90.0f;  // 6도씩 + 초단위 보간
    clock->hourAngle = (clock->hours * 30.0f + clock->minutes * 0.5f) - 90.0f;  // 30도씩 + 분단위 보간
}

// 시계 바늘을 그리는 함수
void DrawClockHand(Vector2 center, float angle, float length, float thickness, Color color) {
    float endX = center.x + cosf(angle * DEG2RAD) * length;
    float endY = center.y + sinf(angle * DEG2RAD) * length;

    // 본체 그리기
    DrawLineEx(center, {endX, endY}, thickness, color);
    DrawCircle(center.x, center.y, thickness / 2, color);
}

// 시계 숫자와 눈금을 그리는 함수 (작은 화면에 맞게 조정)
void DrawClockFace(Vector2 center, float radius) {
    // 외곽 원
    DrawCircleLines(center.x, center.y, radius, BLACK);
    DrawCircleLines(center.x, center.y, radius - 1, BLACK);

    // 시간 숫자와 주요 눈금
    for (int i = 1; i <= 12; i++) {
        float angle = (i * 30.0f) - 90.0f;  // 12시 방향을 0도로
        float numberRadius = radius - 16;  // 작은 화면에 맞게 조정
        float tickRadius = radius - 3;     // 작은 화면에 맞게 조정

        // 주요 눈금 (시간)
        float tickStartX = center.x + cosf(angle * DEG2RAD) * tickRadius;
        float tickStartY = center.y + sinf(angle * DEG2RAD) * tickRadius;
        float tickEndX = center.x + cosf(angle * DEG2RAD) * (tickRadius - 8);
        float tickEndY = center.y + sinf(angle * DEG2RAD) * (tickRadius - 8);

        DrawLineEx({tickStartX, tickStartY}, {tickEndX, tickEndY}, 2, BLACK);

        // 숫자 그리기 (더 작은 폰트 사용)
        char numberStr[3];
        sprintf(numberStr, "%d", i);
        float numberX = center.x + cosf(angle * DEG2RAD) * numberRadius;
        float numberY = center.y + sinf(angle * DEG2RAD) * numberRadius;
        int textWidth = MeasureText(numberStr, 10);
        DrawText(numberStr, numberX - textWidth / 2, numberY - 4, 8, BLACK);
    }

    // 분 눈금 (작은 눈금) - 간격을 넓혀서 보기 좋게
    for (int i = 0; i < 60; i++) {
        if (i % 5 != 0) {  // 5분 단위가 아닌 경우만
            float angle = (i * 6.0f) - 90.0f;
            float tickRadius = radius - 3;

            float tickStartX = center.x + cosf(angle * DEG2RAD) * tickRadius;
            float tickStartY = center.y + sinf(angle * DEG2RAD) * tickRadius;
            float tickEndX = center.x + cosf(angle * DEG2RAD) * (tickRadius - 4);
            float tickEndY = center.y + sinf(angle * DEG2RAD) * (tickRadius - 4);

            DrawLineEx({tickStartX, tickStartY}, {tickEndX, tickEndY}, 0.5f, GRAY);
        }
    }
}

// 시계 전체를 그리는 함수
void DrawAnalogClock(const AnalogClock* clock) {
    // 시계판 배경 (투명한 흰색)
    DrawCircle(clock->center.x, clock->center.y, clock->radius, Fade(WHITE, 0.8f));

    // 시계 숫자와 눈금
    DrawClockFace(clock->center, clock->radius);

    // 시계 바늘 그리기 (뒤에서부터)
    // 시침
    DrawClockHand(clock->center, clock->hourAngle, clock->hourHandLength, 2.0f, BLACK);

    // 분침
    DrawClockHand(clock->center, clock->minuteAngle, clock->minuteHandLength, 1.5f, BLACK);

    // 초침
    DrawClockHand(clock->center, clock->secondAngle, clock->secondHandLength, 1.0f, RED);

    // 중앙 점
    DrawCircle(clock->center.x, clock->center.y, 3, BLACK);
}

int main() {
    // 창을 투명하게 설정 (InitWindow 전에 호출)
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_TOPMOST);

    // 윈도우 초기화
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mini Analog Clock");

    SetTargetFPS(60);

    // 아날로그 시계 초기화
    AnalogClock clock = {0};
    clock.center = CLOCK_CENTER;
    clock.radius = CLOCK_RADIUS;
    clock.hourHandLength = CLOCK_RADIUS * 0.45f;   // 작게 조정
    clock.minuteHandLength = CLOCK_RADIUS * 0.65f; // 작게 조정
    clock.secondHandLength = CLOCK_RADIUS * 0.8f;  // 작게 조정

    // 메인 루프
    while (!WindowShouldClose()) {
        // 현재 시간 업데이트
        UpdateTime(&clock);

        // 렌더링 시작
        BeginDrawing();
        ClearBackground(BLANK);  // 완전 투명 배경

        // 시계 그리기
        DrawAnalogClock(&clock);

        // 디지털 시간 표시 (하단, 더 작은 폰트로 조정)
        char timeStr[32];
        sprintf(timeStr, "%02d:%02d:%02d",
                (clock.hours == 0) ? 12 : clock.hours,
                clock.minutes,
                clock.seconds);
        int textWidth = MeasureText(timeStr, 14);
        DrawText(timeStr, (SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT - 12, 10, DARKGRAY);

        // 제목 제거 또는 매우 작게 표시 (공간 절약을 위해)
        // 200x200 픽셀에서는 공간이 매우 제한적이므로 제목을 생략

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

