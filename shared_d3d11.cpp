#include "shared_d3d11.hpp"

#include <wrl/client.h>
#include <windows.h>
#include <iostream>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

using Microsoft::WRL::ComPtr;

ComPtr<ID3D11Device> lDevice;
ComPtr<ID3D11DeviceContext> lImmediateContext;
ComPtr<IDXGISurface1> gdiSurface;
ComPtr<ID3D11Texture2D> texture;
D3D11_TEXTURE2D_DESC desc;

uint32_t width = 0;
uint32_t height = 0;

D3D_DRIVER_TYPE gDriverTypes[] = {
    D3D_DRIVER_TYPE_HARDWARE
};
UINT gNumDriverTypes = ARRAYSIZE(gDriverTypes);

D3D_FEATURE_LEVEL gFeatureLevels[] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_1
};
UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);

HWND game_window;

bool InitColor() {
    game_window = FindWindowW(NULL, L"Moonlight");
    if (!game_window) return false;

    RECT rect;
    GetClientRect(game_window, &rect);
    width = 416;
    height = 416;

    HRESULT hr(E_FAIL);
    D3D_FEATURE_LEVEL lFeatureLevel;

    for (UINT i = 0; i < gNumDriverTypes; ++i) {
        hr = D3D11CreateDevice(nullptr, gDriverTypes[i], nullptr, 0,
            gFeatureLevels, gNumFeatureLevels, D3D11_SDK_VERSION,
            &lDevice, &lFeatureLevel, &lImmediateContext);
        if (SUCCEEDED(hr)) break;
        lDevice.Reset();
        lImmediateContext.Reset();
    }

    desc.Width = width;
    desc.Height = height;
    desc.ArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
    desc.CPUAccessFlags = 0;

    hr = lDevice->CreateTexture2D(&desc, NULL, &texture);
    if (FAILED(hr)) return false;

    hr = texture->QueryInterface(__uuidof(IDXGISurface1), (void**)&gdiSurface);
    if (FAILED(hr)) return false;

    desc.BindFlags = 0;
    desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;

    std::cout << "Using 416x416 window capture" << std::endl;
    return true;
}

bool ScreenGrab(char** outData) {
    HDC hdc_target = GetDC(game_window);
    HDC hDC = nullptr;

    gdiSurface->GetDC(true, &hDC);

    RECT rect;
    GetClientRect(game_window, &rect);
    int winWidth = rect.right - rect.left;
    int winHeight = rect.bottom - rect.top;

    int offsetX = (winWidth - width) / 2;
    int offsetY = (winHeight - height) / 2;

    if (!BitBlt(hDC, 0, 0, width, height, hdc_target, offsetX, offsetY, SRCCOPY)) {
        std::cout << "BitBlt failed" << std::endl;
        return false;
    }

    ReleaseDC(game_window, hdc_target);
    gdiSurface->ReleaseDC(nullptr);

    ID3D11Texture2D* frameCopy = nullptr;
    HRESULT hr = lDevice->CreateTexture2D(&desc, nullptr, &frameCopy);
    if (FAILED(hr)) return false;

    lImmediateContext->CopyResource(frameCopy, texture.Get());

    D3D11_MAPPED_SUBRESOURCE mapped;
    hr = lImmediateContext->Map(frameCopy, 0, D3D11_MAP_READ, 0, &mapped);
    if (FAILED(hr)) return false;

    *outData = reinterpret_cast<char*>(mapped.pData);

    if (frameCopy) {
        lImmediateContext->Unmap(frameCopy, 0);
        frameCopy->Release();
        lImmediateContext->Flush();
    }

    return true;
}
