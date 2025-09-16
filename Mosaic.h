#pragma once
#include "main.h"
#include "renderer.h"

class Mosaic
{
public:
    Mosaic();
    ~Mosaic();

    HRESULT Initialize();
    void Finalize();

    void Update();

    // 1pass: set offscreen RT and clear
    void BeginScene();
    // 2pass: draw mosaic-processed fullscreen quad to backbuffer
    void EndSceneAndDraw();

    void SetRectSize(int size);
    int  GetRectSize() const { return m_RectSize; }

private:
    HRESULT CreateOffscreenTargets();
    HRESULT CreateFullScreenQuad();
    HRESULT CreateShaders();

    ID3D11Texture2D*        m_OffscreenTex = nullptr;
    ID3D11RenderTargetView* m_OffscreenRTV = nullptr;
    ID3D11ShaderResourceView* m_OffscreenSRV = nullptr;

    ID3D11Texture2D*        m_OffscreenDepth = nullptr;
    ID3D11DepthStencilView* m_OffscreenDSV = nullptr;

    ID3D11VertexShader*     m_VS = nullptr;
    ID3D11PixelShader*      m_PS = nullptr;
    ID3D11InputLayout*      m_InputLayout = nullptr;

    ID3D11Buffer*           m_VertexBuffer = nullptr;

    int m_RectSize = 16; // default
};
