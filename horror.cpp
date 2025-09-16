#include "horror.h"
#include <d3dcompiler.h>

struct FSVertex
{
    XMFLOAT3 pos;
    XMFLOAT2 uv;
};

// CPU mirror of HorrorParams cbuffer
struct HorrorCB
{
    XMFLOAT2 NoiseSeed;
    float    NoiseScale;
    float    GrayScaleEnabled;
    float    ContrastPower;
    XMFLOAT2 NoiseAddPerFrame;
    float    pad[1]; // padding to 16-byte alignment
};

Horror::Horror() {}
Horror::~Horror() { Finalize(); }

HRESULT Horror::Init()
{
    HRESULT hr = S_OK;
    hr = CreateOffscreenTargets();
    if (FAILED(hr)) return hr;

    hr = CreateFullScreenQuad();
    if (FAILED(hr)) return hr;

    hr = CreateShadersAndStates();
    if (FAILED(hr)) return hr;

    return S_OK;
}

void Horror::Finalize()
{
    if (m_VertexBuffer) { m_VertexBuffer->Release(); m_VertexBuffer = nullptr; }
    if (m_InputLayout) { m_InputLayout->Release(); m_InputLayout = nullptr; }
    if (m_VS) { m_VS->Release(); m_VS = nullptr; }
    if (m_PS) { m_PS->Release(); m_PS = nullptr; }

    if (m_OffscreenDSV) { m_OffscreenDSV->Release(); m_OffscreenDSV = nullptr; }
    if (m_OffscreenDepth) { m_OffscreenDepth->Release(); m_OffscreenDepth = nullptr; }

    if (m_OffscreenSRV) { m_OffscreenSRV->Release(); m_OffscreenSRV = nullptr; }
    if (m_OffscreenRTV) { m_OffscreenRTV->Release(); m_OffscreenRTV = nullptr; }
    if (m_OffscreenTex) { m_OffscreenTex->Release(); m_OffscreenTex = nullptr; }
}

HRESULT Horror::CreateOffscreenTargets()
{
    auto device = GetDevice();

    // Color texture
    D3D11_TEXTURE2D_DESC td = {};
    td.Width = SCREEN_WIDTH;
    td.Height = SCREEN_HEIGHT;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = device->CreateTexture2D(&td, nullptr, &m_OffscreenTex);
    if (FAILED(hr)) return hr;

    hr = device->CreateRenderTargetView(m_OffscreenTex, nullptr, &m_OffscreenRTV);
    if (FAILED(hr)) return hr;

    hr = device->CreateShaderResourceView(m_OffscreenTex, nullptr, &m_OffscreenSRV);
    if (FAILED(hr)) return hr;

    // Depth for offscreen pass
    D3D11_TEXTURE2D_DESC dtd = {};
    dtd.Width = SCREEN_WIDTH;
    dtd.Height = SCREEN_HEIGHT;
    dtd.MipLevels = 1;
    dtd.ArraySize = 1;
    dtd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dtd.SampleDesc.Count = 1;
    dtd.Usage = D3D11_USAGE_DEFAULT;
    dtd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = device->CreateTexture2D(&dtd, nullptr, &m_OffscreenDepth);
    if (FAILED(hr)) return hr;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
    dsvd.Format = dtd.Format;
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    hr = device->CreateDepthStencilView(m_OffscreenDepth, &dsvd, &m_OffscreenDSV);
    if (FAILED(hr)) return hr;

    return S_OK;
}

HRESULT Horror::CreateFullScreenQuad()
{
    FSVertex v[4] = {
        { XMFLOAT3(-1, -1, 0), XMFLOAT2(0, 1) },
        { XMFLOAT3(-1,  1, 0), XMFLOAT2(0, 0) },
        { XMFLOAT3( 1, -1, 0), XMFLOAT2(1, 1) },
        { XMFLOAT3( 1,  1, 0), XMFLOAT2(1, 0) },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(FSVertex) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = v;

    return GetDevice()->CreateBuffer(&bd, &init, &m_VertexBuffer);
}

HRESULT Horror::CreateShadersAndStates()
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(L"shader/horrorVS.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob) { OutputDebugStringA((const char*)errBlob->GetBufferPointer()); errBlob->Release(); }
        return hr;
    }

    hr = GetDevice()->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_VS);
    if (FAILED(hr)) { vsBlob->Release(); return hr; }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = GetDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_InputLayout);
    vsBlob->Release();
    if (FAILED(hr)) return hr;

    // Note: entry point is ps_main to avoid build-time confusion
    hr = D3DCompileFromFile(L"shader/horrorPS.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob) { OutputDebugStringA((const char*)errBlob->GetBufferPointer()); errBlob->Release(); }
        return hr;
    }

    hr = GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_PS);
    if (FAILED(hr)) { psBlob->Release(); return hr; }
    psBlob->Release();

    return S_OK;
}

void Horror::Update()
{
    // Advance noise seed per frame
    m_NoiseSeed.x += m_NoiseAddPerFrame.x;
    m_NoiseSeed.y += m_NoiseAddPerFrame.y;

    // Reset if length too large to avoid long-run banding/precision issues
    float len2 = m_NoiseSeed.x * m_NoiseSeed.x + m_NoiseSeed.y * m_NoiseSeed.y;
    if (len2 > 1000.0f * 1000.0f) // threshold: length > 1000
    {
        m_NoiseSeed = XMFLOAT2(0.0f, 0.0f);
    }
}

void Horror::BeginScene()
{
    auto ctx = GetDeviceContext();
    // Bind offscreen RTV + DSV and clear
    ctx->OMSetRenderTargets(1, &m_OffscreenRTV, m_OffscreenDSV);

    float clear[4] = {0,0,0,1};
    ctx->ClearRenderTargetView(m_OffscreenRTV, clear);
    ctx->ClearDepthStencilView(m_OffscreenDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Horror::EndSceneAndDraw()
{
    auto ctx = GetDeviceContext();

    // Bind backbuffer
    BindBackBuffer();

    // Depth off for fullscreen pass
    SetDepthEnable(false);

    // Set shaders and state
    ctx->IASetInputLayout(m_InputLayout);
    ctx->VSSetShader(m_VS, nullptr, 0);
    ctx->PSSetShader(m_PS, nullptr, 0);

    UINT stride = sizeof(FSVertex);
    UINT offset = 0;
    ctx->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // Bind SRV
    ctx->PSSetShaderResources(0, 1, &m_OffscreenSRV);

    // Update and bind HorrorParams at b7
    HorrorCB cb = {};
    cb.NoiseSeed = m_NoiseSeed;
    cb.NoiseScale = m_NoiseScale;
    cb.GrayScaleEnabled = m_GrayScaleEnabled;
    cb.ContrastPower = m_ContrastPower;
    cb.NoiseAddPerFrame = m_NoiseAddPerFrame;

    // Create a transient constant buffer (small and simple for minimal sample)
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(HorrorCB);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    D3D11_SUBRESOURCE_DATA init = { &cb, 0, 0 };

    ID3D11Buffer* cbuf = nullptr;
    HRESULT hr = GetDevice()->CreateBuffer(&bd, &init, &cbuf);
    if (SUCCEEDED(hr))
    {
        ctx->PSSetConstantBuffers(7, 1, &cbuf);
    }

    // Draw fullscreen quad
    ctx->Draw(4, 0);

    // Cleanup
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    ctx->PSSetShaderResources(0, 1, nullSRV);

    if (cbuf) { cbuf->Release(); }
}
