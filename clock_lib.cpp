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
#define LIB_VERSION 2

// AnalogClock 클래스 구현
AnalogClock::AnalogClock() : center{0, 0}, radius(0), hourHandLength(0), minuteHandLength(0),
                            secondHandLength(0), hourAngle(0), minuteAngle(0), secondAngle(0),
                            hours(0), minutes(0), seconds(0) {
}

void AnalogClock::Initialize(Vector2 center, float radius) {
    this->center = center;
    this->radius = radius;
    this->hourHandLength = radius * 0.45f;
    this->minuteHandLength = radius * 0.65f;
    this->secondHandLength = radius * 0.8f;
    this->hourAngle = 0.0f;
    this->minuteAngle = 0.0f;
    this->secondAngle = 0.0f;
    this->hours = 0;
    this->minutes = 0;
    this->seconds = 0;
}

void AnalogClock::Update() {
    time_t rawTime;
    struct tm* timeInfo;

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    hours = timeInfo->tm_hour % 12;
    minutes = timeInfo->tm_min;
    seconds = timeInfo->tm_sec;

    // 각도 계산 (12시 방향을 0도로 하고 시계방향으로 증가)
    secondAngle = (seconds * 6.0f) - 90.0f;  // 6도씩 증가
    minuteAngle = (minutes * 6.0f + seconds * 0.1f) - 90.0f;  // 6도씩 + 초단위 보간
    hourAngle = (hours * 30.0f + minutes * 0.5f) - 90.0f;  // 30도씩 + 분단위 보간
}

void AnalogClock::DrawHand(Vector2 center, float angle, float length, float thickness, Color color) const {
    float endX = center.x + cosf(angle * DEG2RAD) * length;
    float endY = center.y + sinf(angle * DEG2RAD) * length;

    // 본체 그리기
    DrawLineEx(center, {endX, endY}, thickness, color);
    DrawCircle(center.x, center.y, thickness / 2, color);
}

void AnalogClock::DrawFace(Vector2 center, float radius) const {
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

void AnalogClock::Draw() const {
    // 시계판 배경 (투명한 흰색)
    DrawCircle(center.x, center.y, radius, Fade(WHITE, 0.8f));

    // 시계 숫자와 눈금
    DrawFace(center, radius);

    // 시계 바늘 그리기 (뒤에서부터)
    // 시침
    DrawHand(center, hourAngle, hourHandLength, 3.0f, BLACK);

    // 분침
    DrawHand(center, minuteAngle, minuteHandLength, 2.0f, BLACK);

    // 초침
    DrawHand(center, secondAngle, secondHandLength, 1.0f, RED);

    // 중앙 점
    DrawCircle(center.x, center.y, 3, BLACK);
}

int AnalogClock::GetVersion() const {
    return LIB_VERSION;
}

// Factory functions for hot reload
extern "C" {
    void* CreateInstance() {
        return new AnalogClock();
    }

    void DestroyInstance(void* instance) {
        delete static_cast<AnalogClock*>(instance);
    }

    void* CreateInstanceWithState(void* existingInstance) {
        if (existingInstance) {
            AnalogClock* oldClock = static_cast<AnalogClock*>(existingInstance);
            printf("Copying state from existing instance (version %d)\n", oldClock->GetVersion());
            // return new AnalogClock(*oldClock);  // Use copy constructor is not working in hot reload. So manually copy state.
            AnalogClock* newClock = new AnalogClock();
            newClock->Initialize(oldClock->GetCenter(), oldClock->GetRadius());
            return newClock;
        }
        return new AnalogClock();  // Fallback to default constructor
    }
}
