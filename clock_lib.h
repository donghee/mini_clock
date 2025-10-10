// @file      clock_lib.h
// @author    Donghee Park
//
// Copyright (c) 2024 Donghee Park, all rights reserved

#pragma once

#include <raylib.h>
#include <memory>
#include "hot_reload.h"

// Clock interface for hot reload
class IClock : public HotReload::IReloadable {
public:
    virtual ~IClock() = default;

    // Clock operations
    virtual void Initialize(Vector2 center, float radius) = 0;
    virtual void Update() = 0;
    virtual void Draw() const = 0;
    virtual void DrawFace(Vector2 center, float radius) const = 0;
    virtual void DrawHand(Vector2 center, float angle, float length, float thickness, Color color) const = 0;

    // Get current time values
    virtual int GetHours() const = 0;
    virtual int GetMinutes() const = 0;
    virtual int GetSeconds() const = 0;
};

// Clock implementation
class AnalogClock : public IClock {
private:
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

public:
    AnalogClock();
    AnalogClock(const AnalogClock& other);  // Copy constructor
    virtual ~AnalogClock() = default;

    // IClock interface implementation
    void Initialize(Vector2 center, float radius) override;
    void Update() override;
    void Draw() const override;
    void DrawFace(Vector2 center, float radius) const override;
    void DrawHand(Vector2 center, float angle, float length, float thickness, Color color) const override;
    int GetVersion() const override;

    // Getters
    int GetHours() const override { return hours; }
    int GetMinutes() const override { return minutes; }
    int GetSeconds() const override { return seconds; }

    // State getters for copying
    Vector2 GetCenter() const { return center; }
    float GetRadius() const { return radius; }
    float GetHourAngle() const { return hourAngle; }
    float GetMinuteAngle() const { return minuteAngle; }
    float GetSecondAngle() const { return secondAngle; }
};

// Factory functions for hot reload
extern "C" {
    void* CreateInstance();
    void DestroyInstance(void* instance);
    void* CreateInstanceWithState(void* existingInstance);
}
