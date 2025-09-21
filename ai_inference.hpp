#pragma once
#include <windows.h>
#include <atomic>
#include "engine.hpp"
#include "constants.hpp"

extern std::atomic<float> recoilX;
extern std::atomic<float> recoilY;
extern Detection currentTarget;
extern bool hasLockedTarget;

void AIInferenceThread(YoloInferencer& inferencer);
