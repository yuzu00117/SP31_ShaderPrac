#pragma once
#include "main.h"
#include "renderer.h"

// Posterize post-process: render scene to offscreen RT, then apply per-channel quantization in a fullscreen pass
// 手順メモ: まずはホラー効果(Horror)のファイル(horror.h/.cpp, shader)をコピーしてリネームし、
// クラス名やシェーダーファイル名、定数バッファ項目だけ差し替えると最小構成で動作します（差分実装）。
class Posterize
{
public:
    Posterize();
    ~Posterize();

    HRESULT Init();
    void Finalize();

    void Update();

    // 1st pass: bind offscreen RT and clear
    void BeginScene();
    // 2nd pass: apply posterize effect to backbuffer
    void EndSceneAndDraw();

    // Parameters control
    void SetLevels(int lv) { m_Levels = (lv < 2) ? 2 : lv; }
    void SetContrastPower(float p) { m_ContrastPower = p; }
    void SetGrayScaleEnabled(bool enabled) { m_GrayScaleEnabled = enabled ? 1.0f : 0.0f; }

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

    // CPU-side params (mirrors PosterizeParams cbuffer)
    int      m_Levels = 4;          // default 4
    float    m_ContrastPower = 1.0f; // default 1.0
    float    m_GrayScaleEnabled = 0.0f; // default 0.0 (off)
};
