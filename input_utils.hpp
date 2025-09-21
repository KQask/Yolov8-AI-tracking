#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#define _CRT_SECURE_NO_WARNINGS

extern bool mousePressed;
extern bool aiTargeting;
extern std::atomic<float> recoilX;
extern std::atomic<float> recoilY;

void SetupMouseInput();
bool IsCapsLockOn();
void SendToSerial(const std::string& message);
void sendMouseMovement(int dx, int dy, int screenTargetX = -1, int screenTargetY = -1, int targetX = -1, int targetY = -1);
bool InitSerialPort(const char* portName);
void CloseSerialPort();
int clamp(int value, int min_val, int max_val);
