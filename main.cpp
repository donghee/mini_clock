// @file      main.cpp
// @author    Donghee Park
//
// Copyright (c) 2024 Donghee Park, all rights reserved

#include <raylib.h>
#include <raymath.h>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <dlfcn.h>
#include <sys/stat.h>
#include "clock_lib.h"

// 화면 크기 설정 (200x200으로 축소)
const int SCREEN_WIDTH = 200;
const int SCREEN_HEIGHT = 200;

// 시계 설정 (작은 화면에 맞게 조정)
const int CLOCK_RADIUS = 80;
const Vector2 CLOCK_CENTER = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};

// Hot reload를 위한 라이브러리 관리 구조체
struct ClockLibrary {
    void* handle;
    InitClockFunc InitClock;
    UpdateTimeFunc UpdateTime;
    DrawAnalogClockFunc DrawAnalogClock;
    GetLibVersionFunc GetLibVersion;
    time_t lastModTime;
    int version;
};

// 라이브러리 파일의 수정 시간 가져오기
time_t GetFileModTime_(const char* path) {
    struct stat fileStat;
    if (stat(path, &fileStat) == 0) {
        return fileStat.st_mtime;
    }
    return 0;
}

// 라이브러리 로드 또는 리로드
bool LoadClockLibrary(ClockLibrary* lib, const char* libPath) {
    // 기존 라이브러리가 있으면 언로드
    if (lib->handle != nullptr) {
        dlclose(lib->handle);
        lib->handle = nullptr;
        printf("Unloaded previous library\n");
    }

    // 라이브러리 로드 (RTLD_GLOBAL로 메인의 symbols를 공유)
    printf("Loading library from: %s\n", libPath);
    lib->handle = dlopen(libPath, RTLD_NOW | RTLD_GLOBAL);
    if (!lib->handle) {
        fprintf(stderr, "Failed to load library: %s\n", dlerror());
        return false;
    }
    printf("Library loaded successfully\n");

    // 함수 포인터 로드
    printf("Loading function pointers...\n");
    lib->InitClock = (InitClockFunc)dlsym(lib->handle, "InitClock");
    lib->UpdateTime = (UpdateTimeFunc)dlsym(lib->handle, "UpdateTime");
    lib->DrawAnalogClock = (DrawAnalogClockFunc)dlsym(lib->handle, "DrawAnalogClock");
    lib->GetLibVersion = (GetLibVersionFunc)dlsym(lib->handle, "GetLibVersion");

    if (!lib->InitClock || !lib->UpdateTime || !lib->DrawAnalogClock || !lib->GetLibVersion) {
        fprintf(stderr, "Failed to load library functions: %s\n", dlerror());
        fprintf(stderr, "  InitClock: %p\n", (void*)lib->InitClock);
        fprintf(stderr, "  UpdateTime: %p\n", (void*)lib->UpdateTime);
        fprintf(stderr, "  DrawAnalogClock: %p\n", (void*)lib->DrawAnalogClock);
        fprintf(stderr, "  GetLibVersion: %p\n", (void*)lib->GetLibVersion);
        dlclose(lib->handle);
        lib->handle = nullptr;
        return false;
    }
    printf("All function pointers loaded\n");

    // 라이브러리 버전과 수정 시간 업데이트
    lib->version = lib->GetLibVersion();
    lib->lastModTime = GetFileModTime_(libPath);

    printf("Loaded library version %d\n", lib->version);
    return true;
}

// Hot reload 체크
bool CheckAndReloadLibrary(ClockLibrary* lib, const char* libPath) {
    time_t currentModTime = GetFileModTime_(libPath);

    // 파일이 수정되었는지 확인
    if (currentModTime > lib->lastModTime) {
        printf("Library file changed, reloading...\n");

        // 짧은 대기 시간 (빌드가 완료될 때까지)
        // 실제로는 inotify 등을 사용하는 것이 더 좋지만, 간단하게 구현
        return LoadClockLibrary(lib, libPath);
    }

    return false;
}

int main() {
    // 창을 투명하게 설정 (InitWindow 전에 호출)
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_TOPMOST);

    // 윈도우 초기화
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mini Analog Clock");

    SetTargetFPS(60);

    // 라이브러리 로드
    ClockLibrary clockLib = {0};
    const char* libPath = "./libclock.so";

    if (!LoadClockLibrary(&clockLib, libPath)) {
        fprintf(stderr, "Failed to load clock library. Exiting.\n");
        CloseWindow();
        return 1;
    }

    // 아날로그 시계 초기화
    AnalogClock clock = {0};
    clockLib.InitClock(&clock, CLOCK_CENTER, CLOCK_RADIUS);

    // Hot reload 체크 타이머 (1초마다)
    double lastCheckTime = GetTime();
    const double CHECK_INTERVAL = 1.0;

    // 메인 루프
    while (!WindowShouldClose()) {
        // Hot reload 체크
        double currentTime = GetTime();
        if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
            if (CheckAndReloadLibrary(&clockLib, libPath)) {
                // 라이브러리가 리로드되면 시계 재초기화
                clockLib.InitClock(&clock, CLOCK_CENTER, CLOCK_RADIUS);
            }
            lastCheckTime = currentTime;
        }

        // 현재 시간 업데이트
        clockLib.UpdateTime(&clock);

        // 렌더링 시작
        BeginDrawing();
        ClearBackground(BLANK);  // 완전 투명 배경

        // 시계 그리기
        clockLib.DrawAnalogClock(&clock);

        // 디지털 시간 표시 (하단)
        char timeStr[32];
        sprintf(timeStr, "%02d:%02d:%02d",
                (clock.hours == 0) ? 12 : clock.hours,
                clock.minutes,
                clock.seconds);
        int textWidth = MeasureText(timeStr, 14);
        DrawText(timeStr, (SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT - 12, 10, DARKGRAY);

        // Hot reload 상태 표시
        char versionStr[32];
        sprintf(versionStr, "v%d", clockLib.version);
        DrawText(versionStr, 5, 5, 8, Fade(GREEN, 0.5f));

        EndDrawing();
    }

    // 라이브러리 언로드
    if (clockLib.handle) {
        dlclose(clockLib.handle);
    }

    CloseWindow();

    return 0;
}
