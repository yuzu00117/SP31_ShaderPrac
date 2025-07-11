#include "DrawTextDebug.h"
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <string>
#include <d3d11.h>        // ← d3dDevice, d3dContext に必要
#include <dxgi.h>         // ← IDXGISwapChain に必要
#include <wrl/client.h> // ComPtr用

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

// グローバル
Microsoft::WRL::ComPtr<ID2D1DeviceContext>      g_d2dContext;
Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    g_d2dBrush;
Microsoft::WRL::ComPtr<IDWriteTextFormat>       g_textFormat;
Microsoft::WRL::ComPtr<ID2D1Bitmap1>            g_d2dTargetBitmap;

void InitTextRenderer(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, IDXGISwapChain* swapChain)
{
    // DXGI サーフェス取得（バックバッファ）
    Microsoft::WRL::ComPtr<IDXGISurface> dxgiSurface;
    HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: swapChain->GetBuffer failed\n");
        return;
    }

    // D2D ファクトリの作成
    Microsoft::WRL::ComPtr<ID2D1Factory1> d2dFactory;
    {
        D2D1_FACTORY_OPTIONS options = {};
#ifdef _DEBUG
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

        Microsoft::WRL::ComPtr<ID2D1Factory> tempFactory;
        hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory),
            &options,
            reinterpret_cast<void**>(tempFactory.GetAddressOf())
        );
        if (FAILED(hr))
        {
            OutputDebugStringA("Error: D2D1CreateFactory failed\n");
            return;
        }

        tempFactory.As(&d2dFactory);
    }

    // DXGI デバイス取得
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice);
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: d3dDevice->QueryInterface (IDXGIDevice) failed\n");
        return;
    }

    // D2D デバイス作成
    Microsoft::WRL::ComPtr<ID2D1Device> d2dDevice;
    hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice);
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: d2dFactory->CreateDevice failed\n");
        return;
    }

    // D2D Device Context 作成
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_d2dContext);
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: d2dDevice->CreateDeviceContext failed\n");
        return;
    }

    // D2D ターゲットビットマップを DXGI Surface から作成
    D2D1_BITMAP_PROPERTIES1 bitmapProps =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );
    if (!dxgiSurface)
    {
        OutputDebugStringA("Error: dxgiSurface is NULL!\n");
        return;
    }

    hr = g_d2dContext->CreateBitmapFromDxgiSurface(
        dxgiSurface.Get(),
        &bitmapProps,
        &g_d2dTargetBitmap
    );
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: CreateBitmapFromDxgiSurface failed\n");
        return;
    }

    g_d2dContext->SetTarget(g_d2dTargetBitmap.Get());

    // DirectWrite ファクトリ作成
    Microsoft::WRL::ComPtr<IDWriteFactory> dwriteFactory;
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf())
    );
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: DWriteCreateFactory failed\n");
        return;
    }

    // テキストフォーマット作成
    hr = dwriteFactory->CreateTextFormat(
        L"Meiryo", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        20.0f, L"ja-jp",
        &g_textFormat
    );
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: CreateTextFormat failed\n");
        return;
    }

    // 白色のブラシ作成
    hr = g_d2dContext->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &g_d2dBrush
    );
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: CreateSolidColorBrush failed\n");
        return;
    }

    OutputDebugStringA("InitTextRenderer: 成功しました！\n");
}

void DrawTextDebug(const char* text)
{
    if (!g_d2dContext) return; // ← これを先頭に追加！！
    wchar_t wtext[512];
    MultiByteToWideChar(CP_ACP, 0, text, -1, wtext, 512);

    g_d2dContext->BeginDraw();
    g_d2dContext->DrawText(
        wtext,
        (UINT32)wcslen(wtext),
        g_textFormat.Get(),
        D2D1::RectF(10, 10, 600, 300),  // 高さを200から300に拡張
        g_d2dBrush.Get()
    );
    g_d2dContext->EndDraw();
}

// 位置指定可能なDrawTextDebug関数を追加
void DrawTextDebugAtPosition(const char* text, float x, float y, float width, float height)
{
    if (!g_d2dContext) return;
    wchar_t wtext[512];
    MultiByteToWideChar(CP_ACP, 0, text, -1, wtext, 512);

    g_d2dContext->BeginDraw();
    g_d2dContext->DrawText(
        wtext,
        (UINT32)wcslen(wtext),
        g_textFormat.Get(),
        D2D1::RectF(x, y, x + width, y + height),
        g_d2dBrush.Get()
    );
    g_d2dContext->EndDraw();
}

void ReleaseTextRender()
{
    if (g_d2dBrush) g_d2dBrush.Reset();
    if (g_textFormat) g_textFormat.Reset();
    if (g_d2dTargetBitmap) g_d2dTargetBitmap.Reset();
    if (g_d2dContext) g_d2dContext.Reset();
}