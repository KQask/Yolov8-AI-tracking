# Valorant AI Assist & Tracking

An experimental AI-assisted aim and tracking system for Valorant. Built entirely in C++ with ONNX Runtime CUDA support. Utilizes a custom Yolov8 trained model for inference. Currently undetected and bypasses Vanguard simply by utilizing two PC's. One will be utilized for the actual game, and the other will run the mouse emulation and the AI inference. 
The mouse inputs will be passed over through to the Arduino Leonardo which is replicated on the other end PC. This solution produces close to zero latency mouse movements and inputs. The Moonlight/Sunshine client or OBS with NDI plugin and a network switch allows access to the other PC's screens with relatively low latency.

## Features
- High-performance screen capture (currently being re-coded for better FPS and lower latency)
- AI-based tracking module (prototype)
- Colorbot integration (in progress)
- Smoothing, FOV, and recoil control system (RCS)
- Mouse emulation passed through Arduino to the second PC

## Showcase
[Watch the showcase video here](https://streamable.com/1y7u62)

## Requirements
- C++20 compliant compiler (MSVC or g++)
- OpenCV 4.x
- ONNX Runtime with CUDA support
- `onnxruntime_providers_cuda.dll` must be in the project directory or available in PATH
- Arduino Leonardo
- Two PC's one being capable of running Valorant, the other being capable of AI inference using NVIDIA CUDA
- Network switch to ensure a stable and fast connection between the two PC's

## Roadmap
- [x] Initial prototype with basic tracking
- [x] Expand AI inference with ONNX models
- [ ] Re-code screen capture for improved performance
- [ ] Add Colorbot module

## Build Instructions
```bash
git clone https://github.com/KQask/Yolov8-AI-Tracking.git
cd Yolov8-AI-Tracking
mkdir build && cd build
cmake ..
cmake --build .
```

## Disclaimer
I do not condone the usage of this in live games. This is purely as a proof of concept and I do not encourage using this in live games. 
