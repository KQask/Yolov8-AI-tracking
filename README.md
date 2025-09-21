# Valorant AI Assist & Tracking

An experimental AI-assisted aim and tracking system for Valorant. Built entirely in C++ with ONNX Runtime CUDA support.

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

## Roadmap
- [x] Initial prototype with basic tracking
- [x] Expand AI inference with ONNX models
- [ ] Re-code screen capture for improved performance
- [ ] Add Colorbot module

## Build Instructions
```bash
git clone https://github.com/yourusername/valorant-ai-assist.git
cd valorant-ai-assist
mkdir build && cd build
cmake ..
cmake --build .
```

## Disclaimer
I do not condone the usage of this in live games. This is purely as a proof of concept and I do not encourage using this in live games. 
