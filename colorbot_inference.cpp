#include "colorbot_inference.hpp"
#include "input_utils.hpp"
#include "constants.hpp"
#include "shared_state.hpp"
#include "shared_d3d11.hpp"

#include <thread>
#include <atomic>
#include <iostream>
#include <list>
#include <cmath>
#include <chrono>

bool useBlueFireSorter = true;
int maxX = 600;
int maxY = 300;

struct Vector2 {
    int x, y;
    Vector2(int X, int Y) : x(X), y(Y) {}
    float Len() const {
        return sqrtf(static_cast<float>(x * x + y * y));
    }
    Vector2 operator+(const Vector2& a) const {
        return Vector2(x + a.x, y + a.y);
    }
};

bool IsPurpleColor(unsigned short r, unsigned short g, unsigned short b) {
    if (g >= 170) return false;
    if (g >= 120) return abs(r - b) <= 8 && r - g >= 50 && b - g >= 50 && r >= 105 && b >= 105;
    return abs(r - b) <= 13 && r - g >= 60 && b - g >= 60 && r >= 110 && b >= 100;
}

void FirstColorSorting(char* data, int height, int width) {
    int hWidth = width / 2;
    int hHeight = height / 2;
    for (int y = hHeight - maxY; y < hHeight + maxY; y++) {
        for (int x = hWidth - maxX; x < hWidth + maxX; x++) {
            int base = (x + y * width) * 4;
            unsigned short r = data[base + 2] & 255;
            unsigned short g = data[base + 1] & 255;
            unsigned short b = data[base] & 255;
            if (IsPurpleColor(r, g, b)) {
                sendMouseMovement(x - hWidth, y - hHeight, hWidth, hHeight, x, y);
                return;
            }
        }
    }
}

void BlueFireColorSorting(char* data, int height, int width) {
    const int maxCount = 5;
    const int forSize = 100;
    std::list<Vector2> vects;
    int hWidth = width / 2;
    int hHeight = height / 2;

    for (int y = hHeight - maxY; y < hHeight + maxY; y++) {
        for (int x = hWidth - maxX; x < hWidth + maxX; x++) {
            int base = (x + y * width) * 4;
            unsigned short r = data[base + 2] & 255;
            unsigned short g = data[base + 1] & 255;
            unsigned short b = data[base] & 255;
            if (IsPurpleColor(r, g, b)) {
                vects.emplace_back(x - hWidth, y - hHeight);
            }
        }
    }

    if (!vects.empty()) {
        vects.sort([](const Vector2& a, const Vector2& b) {
            return a.y < b.y;
            });

        std::list<Vector2> filtered;
        for (const auto& v : vects) {
            if (abs(v.x) > maxX || abs(v.y) > maxY) continue;
            bool valid = true;
            for (const auto& f : filtered) {
                if ((v + f).Len() < forSize || abs(v.x + f.x) < forSize) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                filtered.push_front(v);
                if (filtered.size() >= maxCount) break;
            }
        }

        if (!filtered.empty()) {
            filtered.sort([](const Vector2& a, const Vector2& b) {
                return a.x * a.x + a.y * a.y * 100 < b.x * b.x + b.y * b.y * 100;
                });
            Vector2 target = filtered.front();
            sendMouseMovement(target.x, target.y, width / 2, height / 2, target.x + hWidth, target.y + hHeight);
        }
    }
}

void ColorbotThread() {
    if (!InitColor()) return;
    std::cout << "Using resolution: " << width << "x" << height << std::endl;

    while (true) {
        if (!runColorbotThread.load()) {
            Sleep(10);
            continue;
        }

        auto start = std::chrono::high_resolution_clock::now();

        char* data = nullptr;
        if (!ScreenGrab(&data)) continue;

        if (mousePressed) {
            if (useBlueFireSorter) {
                BlueFireColorSorting(data, height, width);
            }
            else {
                FirstColorSorting(data, height, width);
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "Frame time: " << elapsed.count() << " ms" << std::endl;

        Sleep(1);
    }
}
