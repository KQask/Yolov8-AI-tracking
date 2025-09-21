#pragma once

#include <thread>
#include <atomic>

extern std::atomic<bool> runColorbotThread;

void ColorbotThread();
