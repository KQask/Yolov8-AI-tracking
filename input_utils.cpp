#include "input_utils.hpp"


HANDLE hSerial;

bool InitSerialPort(const char* portName) {
    hSerial = CreateFile(portName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    return hSerial != INVALID_HANDLE_VALUE;
}

void SendToSerial(const std::string& message) {
    if (hSerial != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten;
        WriteFile(hSerial, message.c_str(), message.length(), &bytesWritten, NULL);
        FlushFileBuffers(hSerial);
    }
}

int clamp(int value, int min_val, int max_val) {
    return (value < min_val) ? min_val : (value > max_val) ? max_val : value;
}

void sendMouseMovement(int dx, int dy, int screenTargetX, int screenTargetY, int targetX, int targetY) {
    char buffer[64];
    std::cout << "Sending: M " << dx << " " << dy << "\n";

    sprintf_s(buffer, "M %d %d\n", clamp(dx, -127, 127), clamp(dy, -127, 127));
    SendToSerial(buffer);
}

void CloseSerialPort() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }
}
