#include "ai_inference.hpp"
#include "constants.hpp"
#include <opencv2/opencv.hpp>
#include <mutex>
#include <atomic>
#include <windows.h>
#include "shared_state.hpp"
#include "colorbot_inference.hpp"
#include "shared_d3d11.hpp"

std::mutex aiMutex;
Detection currentTarget;
bool hasLockedTarget = false;


void AIInferenceThread(YoloInferencer& inferencer) {
    if (!InitColor()) return;

    while (true) {
        if (!runAIThread.load()) {
            Sleep(10);
            continue;
        }

        auto capStart = std::chrono::high_resolution_clock::now();
        char* rawData = nullptr;
        if (!ScreenGrab(&rawData)) continue;
        //cv::Mat debug_frame(height, width, CV_8UC4, rawData);
        //cv::cvtColor(debug_frame, debug_frame, cv::COLOR_BGRA2BGR);
        //cv::imshow("debug_frame", debug_frame);
        //cv::waitKey(1);

        cv::Mat frame(height, width, CV_8UC4, rawData);
        cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);
        auto capEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> capTime = capEnd - capStart;
        std::cout << "Capture time: " << capTime.count() << " ms" << std::endl;

        if (frame.empty()) continue;

        auto start = std::chrono::high_resolution_clock::now();
        std::vector<Detection> detections = inferencer.infer(frame, 0.3, 0.5);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> frameTime = end - start;
        std::cout << "AI Frame time: " << frameTime.count() << " ms" << std::endl;

        Detection target = currentTarget;
        bool found = false;
        int closestDist = INT_MAX;

        for (const auto& det : detections) {
            int cx = det.box.x + det.box.width / 2;
            int cy = det.box.y + det.box.height / 2;
            int dx = cx - 208;
            int dy = cy - 208;
            int dist = dx * dx + dy * dy;

            bool isSameAsLast = abs(det.box.x - currentTarget.box.x) < 15 &&
                abs(det.box.y - currentTarget.box.y) < 15 &&
                det.class_id == currentTarget.class_id;

            if (isSameAsLast) {
                target = det;
                found = true;
                break;
            }

            if (!found || dist < closestDist) {
                closestDist = dist;
                target = det;
                found = true;
            }
        }


        {
            std::lock_guard<std::mutex> lock(aiMutex);
            currentTarget = target;
            hasLockedTarget = found;
        }
        

    }
}
