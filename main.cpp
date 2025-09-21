#define _CRT_SECURE_NO_WARNINGS
#include "engine.hpp"
#include "ai_inference.hpp"
#include "colorbot_inference.hpp"
#include "input_utils.hpp"
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <iostream>
#include <thread>
#include <atomic>



#define LOCK_X 800
#define LOCK_Y 450
#define AIM_DEADZONE 6

float movementSmoothing = 0.02f;
float recoilStrength = 1.5f;
float recoilDecay = 0.1f;

bool mousePressed = false;
bool aiTargeting = false;
std::atomic<float> recoilX = 0;
std::atomic<float> recoilY = 0;
std::atomic<bool> runAIThread = true;
std::atomic<bool> runColorbotThread = false;

enum AimMode { AI, COLORBOT };
AimMode currentAimMode = AI;

bool IsCapsLockOn() {
    return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INPUT) {
        UINT dwSize;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

        std::vector<BYTE> lpb(dwSize);
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.data(), &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
            RAWINPUT* raw = (RAWINPUT*)lpb.data();

            if (raw->header.dwType == RIM_TYPEMOUSE && IsCapsLockOn()) {
                RAWMOUSE mouseData = raw->data.mouse;

                if (mouseData.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
                    mousePressed = true;
                    SendToSerial("LCLICK\n");
                }

                if (mouseData.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
                    mousePressed = false;
                    SendToSerial("LRELEASE\n");
                }

                if (mouseData.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
                    SendToSerial("RCLICK\n");
                }

                if (mouseData.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
                    SendToSerial("RRELEASE\n");
                }

                if (mouseData.usFlags == MOUSE_MOVE_RELATIVE) {
                    recoilX = static_cast<float>(mouseData.lLastX) * recoilStrength;
                    recoilY = static_cast<float>(mouseData.lLastY) * recoilStrength;
                    aiTargeting = false;
                    sendMouseMovement(mouseData.lLastX, mouseData.lLastY);
                    SetCursorPos(LOCK_X, LOCK_Y);
                }
            }
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void SetupMouseInput() {
    const char CLASS_NAME[] = "HIDMouseInterceptWindow";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "HID Mouse Intercept", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, NULL, NULL,
        GetModuleHandle(NULL), NULL);

    ShowWindow(hwnd, SW_HIDE);
    SetForegroundWindow(hwnd);

    RAWINPUTDEVICE rid = { 0x01, 0x02, RIDEV_INPUTSINK, hwnd };
    RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

int main() {
    if (!InitSerialPort("COM3")) {
		std::cerr << "Failed to open serial port" << std::endl;
		return -1;
    }
    SetupMouseInput();

    std::wstring modelPath = L"best.onnx";
    YoloInferencer inferencer(modelPath, "yolo_inference", "GPU");

    std::thread aiThread(AIInferenceThread, std::ref(inferencer));

    std::thread colorbotThread(ColorbotThread);

    MSG msg;
    while (true) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        static bool wasF1Down = false;
        bool isF1Down = (GetAsyncKeyState(VK_F1) & 0x8000);

        if (isF1Down && !wasF1Down) {
            currentAimMode = (currentAimMode == AI) ? COLORBOT : AI;
            runAIThread = (currentAimMode == AI);
            runColorbotThread = (currentAimMode == COLORBOT);
            std::cout << "Switched to " << (currentAimMode == AI ? "AI Assist" : "Colorbot") << std::endl;
        }

        wasF1Down = isF1Down;


        if (mousePressed && currentAimMode == AI && hasLockedTarget) {
            int targetX = currentTarget.box.x + currentTarget.box.width / 2;
            int targetY = (currentTarget.class_id == 1 || currentTarget.class_id == 0)
                ? currentTarget.box.y + currentTarget.box.height / 4
                : currentTarget.box.y + currentTarget.box.height / 2;

            int rawDx = targetX - SQUARE_SIZE / 2;
            int rawDy = targetY - SQUARE_SIZE / 2;

            int dx = (abs(rawDx) > AIM_DEADZONE) ? clamp(static_cast<int>(rawDx * movementSmoothing - recoilX), -127, 127) : 0;
            int dy = (abs(rawDy) > AIM_DEADZONE) ? clamp(static_cast<int>(rawDy * movementSmoothing - recoilY), -127, 127) : 0;

            recoilX.store(recoilX.load() * recoilDecay);
            recoilY.store(recoilY.load() * recoilDecay);

            aiTargeting = true;
            sendMouseMovement(dx, dy, LOCK_X + rawDx, LOCK_Y + rawDy, targetX, targetY);
        }

        Sleep(1);
    }

    runAIThread = false;
    runColorbotThread = false;
    aiThread.join();
    colorbotThread.join();
    CloseSerialPort();
    return 0;
}