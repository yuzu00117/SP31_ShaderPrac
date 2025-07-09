#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

void InitTextRenderer(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, IDXGISwapChain* swapChain);
void DrawTextDebug(const char* text);
void ReleaseTextRender();