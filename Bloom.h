#pragma once
#include "main.h"
#include "renderer.h"

// Bloom post-process: prefilter (threshold+softknee) -> half-res blur (separable) -> composite(add)
class Bloom
{
public:
    Bloom();
    ~Bloom();

    HRESULT Init();
    void Finalize();

    void Update();

    // 1st pass: bind offscreen RT (scene rendering target)
    void BeginScene();
    // Resolve: prefilter + blur at half-res + composite to backbuffer
    void EndSceneAndDraw();

    // Parameters
    void SetThreshold(float t) { m_Threshold = (t < 0.0f) ? 0.0f : t; }
    void SetSoftKnee(float k)  { m_SoftKnee = (k < 0.0f) ? 0.0f : (k > 1.0f ? 1.0f : k); }
    void SetIntensity(float v) { m_Intensity = (v < 0.0f) ? 0.0f : v; }
    void SetSigma(float s)     { m_Sigma = (s <= 0.0f) ? 0.1f : s; }
    void SetRadius(int r)      { m_Radius = (r < 1) ? 1 : (r > 10 ? 10 : r); }

private:
    HRESULT CreateOffscreenTargets();
    HRESULT CreateFullScreenQuad();
    HRESULT CreateShaders();

    struct FSVertex { XMFLOAT3 pos; XMFLOAT2 uv; };

    // Full-res offscreen (scene color + depth)
    ID3D11Texture2D*            m_OffscreenTex = nullptr;
    ID3D11RenderTargetView*     m_OffscreenRTV = nullptr;
    ID3D11ShaderResourceView*   m_OffscreenSRV = nullptr;
    ID3D11Texture2D*            m_OffscreenDepth = nullptr;
    ID3D11DepthStencilView*     m_OffscreenDSV = nullptr;

    // Half-res bloom buffers (ping-pong)
    ID3D11Texture2D*            m_BloomTex = nullptr;
    ID3D11RenderTargetView*     m_BloomRTV = nullptr;
    ID3D11ShaderResourceView*   m_BloomSRV = nullptr;

    ID3D11Texture2D*            m_BloomTempTex = nullptr;
    ID3D11RenderTargetView*     m_BloomTempRTV = nullptr;
    ID3D11ShaderResourceView*   m_BloomTempSRV = nullptr;

    // Shaders and geometry
    ID3D11VertexShader*         m_VS = nullptr;
    ID3D11PixelShader*          m_PSPrefilter = nullptr;
    ID3D11PixelShader*          m_PSBlur = nullptr;      // reuse blurPS.hlsl
    ID3D11PixelShader*          m_PSComposite = nullptr;
    ID3D11InputLayout*          m_InputLayout = nullptr;
    ID3D11Buffer*               m_VertexBuffer = nullptr;

    // Params
    float m_Threshold = 1.0f;
    float m_SoftKnee  = 0.5f;
    float m_Intensity = 0.8f;

    float m_Sigma = 2.0f;
    int   m_Radius = 4;
};
