// @file      main.cpp
// @author    Donghee Park
//
// Copyright (c) 2024 Donghee Park, all rights reserved

#include <raylib.h>
#include <raymath.h>
#include <cstdio>
#include "hot_reload.h"
#include "clock_lib.h"

// 화면 크기 설정 (200x200으로 축소)
const int SCREEN_WIDTH = 200;
const int SCREEN_HEIGHT = 200;

// 시계 설정 (작은 화면에 맞게 조정)
const int CLOCK_RADIUS = 80;
const Vector2 CLOCK_CENTER = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};

int main() {
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_TOPMOST);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mini Analog Clock");
    SetTargetFPS(60);

    // Hot reload 매니저 초기화
    HotReload::Manager<IClock> clockManager;
    const char* libPath = "./libclock.so";

    if (!clockManager.Initialize(libPath)) {
        fprintf(stderr, "Failed to initialize clock library. Exiting.\n");
        CloseWindow();
        return 1;
    }

    // 시계 초기화
    IClock* clock = clockManager.GetInstance();
    if (clock) {
        clock->Initialize(CLOCK_CENTER, CLOCK_RADIUS);
    } 

    // Hot reload 체크 타이머 (1초마다)
    double lastCheckTime = GetTime();
    const double CHECK_INTERVAL = 1.0;

    // 메인 루프
    while (!WindowShouldClose()) {
        // Hot reload 체크
        double currentTime = GetTime();
        if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
            if (clockManager.CheckAndReload()) {
                clock = clockManager.GetInstance();
                if (clock) {
                    clock->Initialize(CLOCK_CENTER, CLOCK_RADIUS);
                }
            }
            lastCheckTime = currentTime;
        }

        // 현재 시간 업데이트
        if (clock) {
            clock->Update();
        }

        // 렌더링 시작
        BeginDrawing();
        ClearBackground(BLANK);  // 완전 투명 배경

        // 시계 그리기
        if (clock) {
            clock->Draw();
        }

        // 디지털 시간 표시 (하단)
        if (clock) {
            char timeStr[32];
            sprintf(timeStr, "%02d:%02d:%02d",
                    (clock->GetHours() == 0) ? 12 : clock->GetHours(),
                    clock->GetMinutes(),
                    clock->GetSeconds());
            int textWidth = MeasureText(timeStr, 14);
            DrawText(timeStr, (SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT - 12, 10, DARKGRAY);
        }

        // Hot reload 상태 표시
        char versionStr[32];
        sprintf(versionStr, "v%d", clockManager.GetVersion());
        DrawText(versionStr, 5, 5, 8, Fade(GREEN, 0.5f));

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
