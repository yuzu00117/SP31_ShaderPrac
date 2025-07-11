#include "DrawTextDebug.h"
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <string>
#include <d3d11.h>        // �� d3dDevice, d3dContext �ɕK�v
#include <dxgi.h>         // �� IDXGISwapChain �ɕK�v
#include <wrl/client.h> // ComPtr�p

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

// �O���[�o��
Microsoft::WRL::ComPtr<ID2D1DeviceContext>      g_d2dContext;
Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    g_d2dBrush;
Microsoft::WRL::ComPtr<IDWriteTextFormat>       g_textFormat;
Microsoft::WRL::ComPtr<ID2D1Bitmap1>            g_d2dTargetBitmap;

void InitTextRenderer(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, IDXGISwapChain* swapChain)
{
    // DXGI �T�[�t�F�X�擾�i�o�b�N�o�b�t�@�j
    Microsoft::WRL::ComPtr<IDXGISurface> dxgiSurface;
    HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: swapChain->GetBuffer failed\n");
        return;
    }

    // D2D �t�@�N�g���̍쐬
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

    // DXGI �f�o�C�X�擾
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice);
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: d3dDevice->QueryInterface (IDXGIDevice) failed\n");
        return;
    }

    // D2D �f�o�C�X�쐬
    Microsoft::WRL::ComPtr<ID2D1Device> d2dDevice;
    hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice);
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: d2dFactory->CreateDevice failed\n");
        return;
    }

    // D2D Device Context �쐬
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_d2dContext);
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: d2dDevice->CreateDeviceContext failed\n");
        return;
    }

    // D2D �^�[�Q�b�g�r�b�g�}�b�v�� DXGI Surface ����쐬
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

    // DirectWrite �t�@�N�g���쐬
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

    // �e�L�X�g�t�H�[�}�b�g�쐬
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

    // ���F�̃u���V�쐬
    hr = g_d2dContext->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &g_d2dBrush
    );
    if (FAILED(hr))
    {
        OutputDebugStringA("Error: CreateSolidColorBrush failed\n");
        return;
    }

    OutputDebugStringA("InitTextRenderer: �������܂����I\n");
}

void DrawTextDebug(const char* text)
{
    if (!g_d2dContext) return; // �� �����擪�ɒǉ��I�I
    wchar_t wtext[512];
    MultiByteToWideChar(CP_ACP, 0, text, -1, wtext, 512);

    g_d2dContext->BeginDraw();
    g_d2dContext->DrawText(
        wtext,
        (UINT32)wcslen(wtext),
        g_textFormat.Get(),
        D2D1::RectF(10, 10, 600, 300),  // ������200����300�Ɋg��
        g_d2dBrush.Get()
    );
    g_d2dContext->EndDraw();
}

// �ʒu�w��\��DrawTextDebug�֐���ǉ�
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