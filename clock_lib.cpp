// @file      clock_lib.cpp
// @author    Donghee Park
//
// Copyright (c) 2024 Donghee Park, all rights reserved

#include "clock_lib.h"
#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <ctime>
#include <cstdio>

// 라이브러리 버전 (변경 시 증가)
#define LIB_VERSION 1

extern "C" {

// 라이브러리 버전 반환
int GetLibVersion() {
    return LIB_VERSION;
}

// 시계 초기화
void InitClock(AnalogClock* clock, Vector2 center, float radius) {
    clock->center = center;
    clock->radius = radius;
    clock->hourHandLength = radius * 0.45f;
    clock->minuteHandLength = radius * 0.65f;
    clock->secondHandLength = radius * 0.8f;
    clock->hourAngle = 0.0f;
    clock->minuteAngle = 0.0f;
    clock->secondAngle = 0.0f;
    clock->hours = 0;
    clock->minutes = 0;
    clock->seconds = 0;
}

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

// 시계 숫자와 눈금을 그리는 함수
void DrawClockFace(Vector2 center, float radius) {
    // 외곽 원
    DrawCircleLines(center.x, center.y, radius, BLACK);
    DrawCircleLines(center.x, center.y, radius - 1, BLACK);

    // 시간 숫자와 주요 눈금
    for (int i = 1; i <= 12; i++) {
        float angle = (i * 30.0f) - 90.0f;  // 12시 방향을 0도로
        float numberRadius = radius - 16;
        float tickRadius = radius - 3;

        // 주요 눈금 (시간)
        float tickStartX = center.x + cosf(angle * DEG2RAD) * tickRadius;
        float tickStartY = center.y + sinf(angle * DEG2RAD) * tickRadius;
        float tickEndX = center.x + cosf(angle * DEG2RAD) * (tickRadius - 8);
        float tickEndY = center.y + sinf(angle * DEG2RAD) * (tickRadius - 8);

        DrawLineEx({tickStartX, tickStartY}, {tickEndX, tickEndY}, 2, BLACK);

        // 숫자 그리기
        char numberStr[3];
        sprintf(numberStr, "%d", i);
        float numberX = center.x + cosf(angle * DEG2RAD) * numberRadius;
        float numberY = center.y + sinf(angle * DEG2RAD) * numberRadius;
        int textWidth = MeasureText(numberStr, 10);
        DrawText(numberStr, numberX - textWidth / 2, numberY - 4, 8, BLACK);
    }

    // 분 눈금 (작은 눈금)
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
    DrawClockHand(clock->center, clock->hourAngle, clock->hourHandLength, 3.0f, BLACK);

    // 분침
    DrawClockHand(clock->center, clock->minuteAngle, clock->minuteHandLength, 2.0f, BLACK);

    // 초침
    DrawClockHand(clock->center, clock->secondAngle, clock->secondHandLength, 1.0f, RED);

    // 중앙 점
    DrawCircle(clock->center.x, clock->center.y, 3, BLACK);
}

} // extern "C"
