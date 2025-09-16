#pragma once
#include "main.h"
#include "renderer.h"

// Horror post-process effect: render scene to offscreen RT, then apply noise/contrast in a fullscreen pass
class Horror
{
public:
    Horror();
    ~Horror();

    HRESULT Init();
    void Finalize();

    void Update();

    // 1st pass: bind offscreen RT and clear
    void BeginScene();
    // 2nd pass: apply horror effect to backbuffer
    void EndSceneAndDraw();

    // Parameters control
    void SetGrayScaleEnabled(bool enabled) { m_GrayScaleEnabled = enabled ? 1.0f : 0.0f; }
    void SetContrastPower(float p) { m_ContrastPower = p; }
    void SetNoiseScale(float s) { m_NoiseScale = s; }
    void SetNoiseAddPerFrame(const XMFLOAT2& add) { m_NoiseAddPerFrame = add; }

private:
    HRESULT CreateOffscreenTargets();
    HRESULT CreateFullScreenQuad();
    HRESULT CreateShadersAndStates();

    // GPU resources
    ID3D11Texture2D*            m_OffscreenTex = nullptr;
    ID3D11RenderTargetView*     m_OffscreenRTV = nullptr;
    ID3D11ShaderResourceView*   m_OffscreenSRV = nullptr;
    ID3D11Texture2D*            m_OffscreenDepth = nullptr;
    ID3D11DepthStencilView*     m_OffscreenDSV = nullptr;

    ID3D11VertexShader*         m_VS = nullptr;
    ID3D11PixelShader*          m_PS = nullptr;
    ID3D11InputLayout*          m_InputLayout = nullptr;
    ID3D11Buffer*               m_VertexBuffer = nullptr;

    // CPU-side params (mirrors HorrorParams cbuffer)
    XMFLOAT2 m_NoiseSeed = XMFLOAT2(0.0f, 0.0f);
    float    m_NoiseScale = 120.0f;
    float    m_GrayScaleEnabled = 1.0f;
    float    m_ContrastPower = 5.0f;
    XMFLOAT2 m_NoiseAddPerFrame = XMFLOAT2(0.01f, 0.013f);

    // Toggle
    bool     m_Enabled = true;
};
