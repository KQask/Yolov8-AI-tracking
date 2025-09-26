# Valorant AI Assist & Tracking

An experimental AI-assisted aim and tracking system for *Valorant*, developed in **C++** with **ONNX Runtime** and **CUDA acceleration**. The system leverages a custom **YOLOv8** model for real-time inference.

To remain undetected by Riot’s Vanguard anti-cheat, the solution operates across **two separate PCs**:  
- **PC 1**: Runs *Valorant*  
- **PC 2**: Handles AI inference, mouse emulation, and input relay

Mouse inputs are transmitted via an **Arduino Leonardo**, ensuring near-zero latency. Game visuals are streamed to the inference PC using either **Moonlight/Sunshine** or **OBS with NDI**, supported by a dedicated network switch for minimal latency.

---

## Features
- High-performance screen capture (currently being reimplemented for improved performance)  
- AI-driven tracking module (prototype)  
- Planned Colorbot integration  
- Customizable smoothing, FOV, and recoil control system (RCS)  
- Low-latency mouse emulation via Arduino passthrough  

---

## Showcase
[Watch the demo](https://streamable.com/1y7u62)

---

## Requirements
- **C++20** compliant compiler (MSVC or g++)  
- **OpenCV 4.x**  
- **ONNX Runtime with CUDA support**  
- `onnxruntime_providers_cuda.dll` in project directory or PATH  
- **Arduino Leonardo**  
- Two PCs:  
  - Gaming PC (running Valorant)  
  - Inference PC (NVIDIA CUDA capable)  
- **Network switch** for low-latency communication  
- **Moonlight/Sunshine** client or **OBS NDI plugin** for video streaming  

---

## Build / Distribution Note
Build instructions, Arduino source code, and any techniques for Arduino spoofing or input injection are **intentionally omitted** from this repository. This omission is deliberate: I did not provide build instructions because I do not want this work to be abused in live games or distributed in a way that enables misuse. The repository likewise does not include Arduino firmware or any method for spoofing Arduino devices. The project is published as a proof-of-concept for research and education only; any practical deployment or distribution would require the user to add those artifacts intentionally.

---

## Roadmap
- [x] Initial prototype with basic AI tracking  
- [x] Integration of ONNX-based inference  
- [ ] Optimized screen capture for higher FPS and reduced latency  
- [ ] Implementation of Colorbot module  
- [ ] Expanded configuration options (smoothing, RCS, FOV customization)  
- [ ] Improved modular architecture for easier feature integration  

---

## Disclaimer
This project is a proof of concept intended solely for educational and research purposes.I do **not** endorse or encourage the use of AI-assisted aiming systems in live games, as doing so violates the terms of service of *Valorant* and may result in permanent bans.

---
