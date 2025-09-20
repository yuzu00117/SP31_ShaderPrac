#include "Bloom.h"
#include <d3dcompiler.h>
#include "keyboard.h"

struct BlurCB
{
    XMFLOAT2 Direction; // (1,0) or (0,1)
    float    Sigma;
    int      Radius;
    XMFLOAT2 TexelSize; // (1/width, 1/height) of current target
    XMFLOAT2 pad;
};

struct BloomPrefilterCB
{
    float Threshold;
    float SoftKnee;
    XMFLOAT2 pad;
};

struct BloomCompositeCB
{
    float Intensity;
    XMFLOAT3 pad;
};

Bloom::Bloom() {}
Bloom::~Bloom() { Finalize(); }

HRESULT Bloom::Init()
{
    HRESULT hr = S_OK;
    hr = CreateOffscreenTargets(); if (FAILED(hr)) return hr;
    hr = CreateFullScreenQuad();   if (FAILED(hr)) return hr;
    hr = CreateShaders();          if (FAILED(hr)) return hr;
    return S_OK;
}

void Bloom::Finalize()
{
    if (m_VertexBuffer) { m_VertexBuffer->Release(); m_VertexBuffer = nullptr; }
    if (m_InputLayout)  { m_InputLayout->Release();  m_InputLayout = nullptr; }
    if (m_VS)           { m_VS->Release();           m_VS = nullptr; }

    if (m_PSPrefilter)  { m_PSPrefilter->Release();  m_PSPrefilter = nullptr; }
    if (m_PSBlur)       { m_PSBlur->Release();       m_PSBlur = nullptr; }
    if (m_PSComposite)  { m_PSComposite->Release();  m_PSComposite = nullptr; }

    if (m_BloomTempSRV) { m_BloomTempSRV->Release(); m_BloomTempSRV = nullptr; }
    if (m_BloomTempRTV) { m_BloomTempRTV->Release(); m_BloomTempRTV = nullptr; }
    if (m_BloomTempTex) { m_BloomTempTex->Release(); m_BloomTempTex = nullptr; }

    if (m_BloomSRV) { m_BloomSRV->Release(); m_BloomSRV = nullptr; }
    if (m_BloomRTV) { m_BloomRTV->Release(); m_BloomRTV = nullptr; }
    if (m_BloomTex) { m_BloomTex->Release(); m_BloomTex = nullptr; }

    if (m_OffscreenDSV) { m_OffscreenDSV->Release(); m_OffscreenDSV = nullptr; }
    if (m_OffscreenDepth){ m_OffscreenDepth->Release(); m_OffscreenDepth = nullptr; }

    if (m_OffscreenSRV) { m_OffscreenSRV->Release(); m_OffscreenSRV = nullptr; }
    if (m_OffscreenRTV) { m_OffscreenRTV->Release(); m_OffscreenRTV = nullptr; }
    if (m_OffscreenTex) { m_OffscreenTex->Release(); m_OffscreenTex = nullptr; }
}

HRESULT Bloom::CreateOffscreenTargets()
{
    auto device = GetDevice();

    // Full-res offscreen
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

    // Depth for scene pass
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

    // Half-res bloom targets
    D3D11_TEXTURE2D_DESC th = td;
    th.Width  = max(1u, SCREEN_WIDTH  / 2);
    th.Height = max(1u, SCREEN_HEIGHT / 2);

    hr = device->CreateTexture2D(&th, nullptr, &m_BloomTex);
    if (FAILED(hr)) return hr;
    hr = device->CreateRenderTargetView(m_BloomTex, nullptr, &m_BloomRTV);
    if (FAILED(hr)) return hr;
    hr = device->CreateShaderResourceView(m_BloomTex, nullptr, &m_BloomSRV);
    if (FAILED(hr)) return hr;

    hr = device->CreateTexture2D(&th, nullptr, &m_BloomTempTex);
    if (FAILED(hr)) return hr;
    hr = device->CreateRenderTargetView(m_BloomTempTex, nullptr, &m_BloomTempRTV);
    if (FAILED(hr)) return hr;
    hr = device->CreateShaderResourceView(m_BloomTempTex, nullptr, &m_BloomTempSRV);
    if (FAILED(hr)) return hr;

    return S_OK;
}

HRESULT Bloom::CreateFullScreenQuad()
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

HRESULT Bloom::CreateShaders()
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(L"shader/blurVS.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, &errBlob);
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

    // Prefilter PS
    hr = D3DCompileFromFile(L"shader/bloomPrefilterPS.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob) { OutputDebugStringA((const char*)errBlob->GetBufferPointer()); errBlob->Release(); }
        return hr;
    }
    hr = GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_PSPrefilter);
    if (FAILED(hr)) { psBlob->Release(); return hr; }
    psBlob->Release();

    // Blur PS (reuse)
    hr = D3DCompileFromFile(L"shader/blurPS.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob) { OutputDebugStringA((const char*)errBlob->GetBufferPointer()); errBlob->Release(); }
        return hr;
    }
    hr = GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_PSBlur);
    if (FAILED(hr)) { psBlob->Release(); return hr; }
    psBlob->Release();

    // Composite PS
    hr = D3DCompileFromFile(L"shader/bloomCompositePS.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob) { OutputDebugStringA((const char*)errBlob->GetBufferPointer()); errBlob->Release(); }
        return hr;
    }
    hr = GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_PSComposite);
    psBlob->Release();
    return hr;
}

void Bloom::Update()
{
    // Optional key controls (example)
    if (Keyboard_IsKeyDownTrigger(KK_RIGHT)) { SetIntensity(m_Intensity + 0.1f); }
    if (Keyboard_IsKeyDownTrigger(KK_LEFT))  { SetIntensity(m_Intensity - 0.1f); }
}

void Bloom::BeginScene()
{
    auto ctx = GetDeviceContext();
    ctx->OMSetRenderTargets(1, &m_OffscreenRTV, m_OffscreenDSV);

    float clear[4] = {0,0,0,1};
    ctx->ClearRenderTargetView(m_OffscreenRTV, clear);
    ctx->ClearDepthStencilView(m_OffscreenDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Bloom::EndSceneAndDraw()
{
    auto ctx = GetDeviceContext();
    SetDepthEnable(false);

    UINT stride = sizeof(FSVertex);
    UINT offset = 0;

    // Common VS/IA
    ctx->IASetInputLayout(m_InputLayout);
    ctx->VSSetShader(m_VS, nullptr, 0);
    ctx->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    ID3D11ShaderResourceView* nullSRV2[2] = { nullptr, nullptr };

    // 1) Prefilter (scene -> half-res bloom)
    ctx->PSSetShader(m_PSPrefilter, nullptr, 0);
    ctx->OMSetRenderTargets(1, &m_BloomRTV, nullptr);
    ctx->PSSetShaderResources(0, 1, &m_OffscreenSRV);

    BloomPrefilterCB pre = { m_Threshold, m_SoftKnee, XMFLOAT2(0,0) };
    D3D11_BUFFER_DESC cbd = {};
    cbd.ByteWidth = sizeof(BloomPrefilterCB);
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    D3D11_SUBRESOURCE_DATA initPre = { &pre, 0, 0 };

    ID3D11Buffer* cbuf = nullptr;
    if (SUCCEEDED(GetDevice()->CreateBuffer(&cbd, &initPre, &cbuf)))
    {
        ctx->PSSetConstantBuffers(9, 1, &cbuf);
    }
    ctx->Draw(4, 0);
    ctx->PSSetShaderResources(0, 1, nullSRV2);
    if (cbuf) { cbuf->Release(); cbuf = nullptr; }

    // 2) Blur (half-res)
    ctx->PSSetShader(m_PSBlur, nullptr, 0);

    const float texW = float(max(1u, SCREEN_WIDTH  / 2));
    const float texH = float(max(1u, SCREEN_HEIGHT / 2));
    XMFLOAT2 texelSize = XMFLOAT2(1.0f / texW, 1.0f / texH);

    // Horizontal
    ctx->OMSetRenderTargets(1, &m_BloomTempRTV, nullptr);
    ctx->PSSetShaderResources(0, 1, &m_BloomSRV);

    BlurCB cbH = { XMFLOAT2(1,0), m_Sigma, m_Radius, texelSize, XMFLOAT2(0,0) };

    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(BlurCB);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    D3D11_SUBRESOURCE_DATA initH = { &cbH, 0, 0 };

    if (SUCCEEDED(GetDevice()->CreateBuffer(&bd, &initH, &cbuf)))
    {
        ctx->PSSetConstantBuffers(8, 1, &cbuf);
    }
    ctx->Draw(4, 0);
    ctx->PSSetShaderResources(0, 1, nullSRV2);
    if (cbuf) { cbuf->Release(); cbuf = nullptr; }

    // Vertical
    ctx->OMSetRenderTargets(1, &m_BloomRTV, nullptr);
    ctx->PSSetShaderResources(0, 1, &m_BloomTempSRV);

    BlurCB cbV = { XMFLOAT2(0,1), m_Sigma, m_Radius, texelSize, XMFLOAT2(0,0) };
    D3D11_SUBRESOURCE_DATA initV = { &cbV, 0, 0 };
    if (SUCCEEDED(GetDevice()->CreateBuffer(&bd, &initV, &cbuf)))
    {
        ctx->PSSetConstantBuffers(8, 1, &cbuf);
    }
    ctx->Draw(4, 0);
    ctx->PSSetShaderResources(0, 1, nullSRV2);
    if (cbuf) { cbuf->Release(); cbuf = nullptr; }

    // 3) Composite to backbuffer
    BindBackBuffer();
    ctx->PSSetShader(m_PSComposite, nullptr, 0);

    ID3D11ShaderResourceView* srvs[2] = { m_OffscreenSRV, m_BloomSRV };
    ctx->PSSetShaderResources(0, 2, srvs);

    BloomCompositeCB comp = { m_Intensity, XMFLOAT3(0,0,0) };
    D3D11_BUFFER_DESC cbd2 = {};
    cbd2.ByteWidth = sizeof(BloomCompositeCB);
    cbd2.Usage = D3D11_USAGE_DEFAULT;
    cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    D3D11_SUBRESOURCE_DATA initC = { &comp, 0, 0 };

    if (SUCCEEDED(GetDevice()->CreateBuffer(&cbd2, &initC, &cbuf)))
    {
        ctx->PSSetConstantBuffers(10, 1, &cbuf);
    }

    ctx->Draw(4, 0);

    // Cleanup
    ID3D11ShaderResourceView* null2[2] = { nullptr, nullptr };
    ctx->PSSetShaderResources(0, 2, null2);
    if (cbuf) { cbuf->Release(); }
}
