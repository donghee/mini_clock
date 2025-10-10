// @file      clock_lib.h
// @author    Donghee Park
//
// Copyright (c) 2024 Donghee Park, all rights reserved

#ifndef CLOCK_LIB_H
#define CLOCK_LIB_H

#include <raylib.h>

#ifdef __cplusplus
extern "C" {
#endif

// 시계 구조체
typedef struct {
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
} AnalogClock;

// 라이브러리 API
typedef void (*InitClockFunc)(AnalogClock* clock, Vector2 center, float radius);
typedef void (*UpdateTimeFunc)(AnalogClock* clock);
typedef void (*DrawAnalogClockFunc)(const AnalogClock* clock);
typedef void (*DrawClockFaceFunc)(Vector2 center, float radius);
typedef void (*DrawClockHandFunc)(Vector2 center, float angle, float length, float thickness, Color color);

// 라이브러리 버전 (Hot reload 감지용)
typedef int (*GetLibVersionFunc)();

// 실제 함수 선언
void InitClock(AnalogClock* clock, Vector2 center, float radius);
void UpdateTime(AnalogClock* clock);
void DrawAnalogClock(const AnalogClock* clock);
void DrawClockFace(Vector2 center, float radius);
void DrawClockHand(Vector2 center, float angle, float length, float thickness, Color color);
int GetLibVersion();

#ifdef __cplusplus
}
#endif

#endif // CLOCK_LIB_H
