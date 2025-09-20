#pragma once
#include "main.h"
#include "renderer.h"

// Gaussian blur post-process (separable 2-pass: horizontal then vertical)
class GaussianBlur
{
public:
    GaussianBlur();
    ~GaussianBlur();

    HRESULT Init();
    void Finalize();

    void Update();

    // 1st pass: bind offscreen RT (scene rendering target)
    void BeginScene();
    // Resolve: run horizontal blur into temp RT, then vertical blur to backbuffer
    void EndSceneAndDraw();

    // Parameters
    void SetSigma(float s) { m_Sigma = (s <= 0.0f) ? 0.1f : s; }
    void SetRadius(int r)  { m_Radius = (r < 1) ? 1 : (r > 10 ? 10 : r); } // clamp 1..10

private:
    HRESULT CreateOffscreenTargets();
    HRESULT CreateFullScreenQuad();
    HRESULT CreateShaders();

    // GPU resources
    ID3D11Texture2D*            m_OffscreenTex = nullptr; // scene color (first pass target)
    ID3D11RenderTargetView*     m_OffscreenRTV = nullptr;
    ID3D11ShaderResourceView*   m_OffscreenSRV = nullptr;
    ID3D11Texture2D*            m_OffscreenDepth = nullptr; // depth for scene pass
    ID3D11DepthStencilView*     m_OffscreenDSV = nullptr;

    // Temp RT for horizontal blur result
    ID3D11Texture2D*            m_TempTex = nullptr;
    ID3D11RenderTargetView*     m_TempRTV = nullptr;
    ID3D11ShaderResourceView*   m_TempSRV = nullptr;

    ID3D11VertexShader*         m_VS = nullptr;
    ID3D11PixelShader*          m_PS = nullptr; // generic PS with direction parameter
    ID3D11InputLayout*          m_InputLayout = nullptr;
    ID3D11Buffer*               m_VertexBuffer = nullptr;

    // Parameters
    float m_Sigma = 2.0f; // standard deviation
    int   m_Radius = 4;   // number of taps to each side (total taps = 2*R+1)
};
