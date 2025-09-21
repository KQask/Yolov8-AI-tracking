#pragma once

#include <windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <cstdint>

extern uint32_t width;
extern uint32_t height;
extern HWND game_window;
extern D3D11_TEXTURE2D_DESC desc;
extern Microsoft::WRL::ComPtr<ID3D11Device> lDevice;
extern Microsoft::WRL::ComPtr<ID3D11DeviceContext> lImmediateContext;
extern Microsoft::WRL::ComPtr<IDXGISurface1> gdiSurface;
extern Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

extern D3D_DRIVER_TYPE gDriverTypes[];
extern UINT gNumDriverTypes;
extern D3D_FEATURE_LEVEL gFeatureLevels[];
extern UINT gNumFeatureLevels;

bool InitColor();
bool ScreenGrab(char** outData);
