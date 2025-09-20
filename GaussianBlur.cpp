#include "GaussianBlur.h"
#include <d3dcompiler.h>
#include "keyboard.h"

struct FSVertex
{
    XMFLOAT3 pos;
    XMFLOAT2 uv;
};

// CPU mirror of cbuffer BlurParams (b8)
struct BlurCB
{
    XMFLOAT2 Direction; // (1,0) horizontal, (0,1) vertical
    float Sigma;        // blur sigma
    int   Radius;       // taps per side
    XMFLOAT2 TexelSize; // (1/width, 1/height)
    XMFLOAT2 pad;
};

GaussianBlur::GaussianBlur() {}
GaussianBlur::~GaussianBlur() { Finalize(); }

HRESULT GaussianBlur::Init()
{
    HRESULT hr = S_OK;
    hr = CreateOffscreenTargets();
    if (FAILED(hr)) return hr;

    hr = CreateFullScreenQuad();
    if (FAILED(hr)) return hr;

    hr = CreateShaders();
    if (FAILED(hr)) return hr;

    return S_OK;
}

void GaussianBlur::Finalize()
{
    if (m_VertexBuffer) { m_VertexBuffer->Release(); m_VertexBuffer = nullptr; }
    if (m_InputLayout)  { m_InputLayout->Release();  m_InputLayout = nullptr; }
    if (m_VS)           { m_VS->Release();           m_VS = nullptr; }
    if (m_PS)           { m_PS->Release();           m_PS = nullptr; }

    if (m_OffscreenDSV) { m_OffscreenDSV->Release(); m_OffscreenDSV = nullptr; }
    if (m_OffscreenDepth){ m_OffscreenDepth->Release(); m_OffscreenDepth = nullptr; }

    if (m_OffscreenSRV) { m_OffscreenSRV->Release(); m_OffscreenSRV = nullptr; }
    if (m_OffscreenRTV) { m_OffscreenRTV->Release(); m_OffscreenRTV = nullptr; }
    if (m_OffscreenTex) { m_OffscreenTex->Release(); m_OffscreenTex = nullptr; }

    if (m_TempSRV) { m_TempSRV->Release(); m_TempSRV = nullptr; }
    if (m_TempRTV) { m_TempRTV->Release(); m_TempRTV = nullptr; }
    if (m_TempTex) { m_TempTex->Release(); m_TempTex = nullptr; }
}

HRESULT GaussianBlur::CreateOffscreenTargets()
{
    auto device = GetDevice();

    // Scene color offscreen
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

    // Temp RT for intermediate blur
    hr = device->CreateTexture2D(&td, nullptr, &m_TempTex);
    if (FAILED(hr)) return hr;
    hr = device->CreateRenderTargetView(m_TempTex, nullptr, &m_TempRTV);
    if (FAILED(hr)) return hr;
    hr = device->CreateShaderResourceView(m_TempTex, nullptr, &m_TempSRV);
    if (FAILED(hr)) return hr;

    return S_OK;
}

HRESULT GaussianBlur::CreateFullScreenQuad()
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

HRESULT GaussianBlur::CreateShaders()
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

    hr = D3DCompileFromFile(L"shader/blurPS.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, &psBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob) { OutputDebugStringA((const char*)errBlob->GetBufferPointer()); errBlob->Release(); }
        return hr;
    }
    hr = GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_PS);
    psBlob->Release();
    return hr;
}

void GaussianBlur::Update()
{
    // Optional: adjust radius with keys
    if (Keyboard_IsKeyDownTrigger(KK_UP)) { SetRadius(m_Radius + 1); }
    if (Keyboard_IsKeyDownTrigger(KK_DOWN)) { SetRadius(m_Radius - 1); }
}

void GaussianBlur::BeginScene()
{
    auto ctx = GetDeviceContext();
    // Bind offscreen RTV + DSV and clear
    ctx->OMSetRenderTargets(1, &m_OffscreenRTV, m_OffscreenDSV);

    float clear[4] = {0,0,0,1};
    ctx->ClearRenderTargetView(m_OffscreenRTV, clear);
    ctx->ClearDepthStencilView(m_OffscreenDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void GaussianBlur::EndSceneAndDraw()
{
    auto ctx = GetDeviceContext();

    // First blur pass: horizontal (to temp RT)
    SetDepthEnable(false);
    UINT stride = sizeof(FSVertex);
    UINT offset = 0;

    // Common VS/IA
    ctx->IASetInputLayout(m_InputLayout);
    ctx->VSSetShader(m_VS, nullptr, 0);
    ctx->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // PS
    ctx->PSSetShader(m_PS, nullptr, 0);

    // Bind dest RT: temp
    ctx->OMSetRenderTargets(1, &m_TempRTV, nullptr);

    // Bind source SRV: offscreen
    ctx->PSSetShaderResources(0, 1, &m_OffscreenSRV);

    // Create and set constant buffer b8 for horizontal pass
    BlurCB cbH = {};
    cbH.Direction = XMFLOAT2(1.0f, 0.0f);
    cbH.Sigma = m_Sigma;
    cbH.Radius = m_Radius;
    cbH.TexelSize = XMFLOAT2(1.0f / SCREEN_WIDTH, 1.0f / SCREEN_HEIGHT);

    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(BlurCB);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    D3D11_SUBRESOURCE_DATA init = { &cbH, 0, 0 };

    ID3D11Buffer* cbuf = nullptr;
    if (SUCCEEDED(GetDevice()->CreateBuffer(&bd, &init, &cbuf)))
    {
        ctx->PSSetConstantBuffers(8, 1, &cbuf);
    }

    ctx->Draw(4, 0);

    // Unbind SRV
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    ctx->PSSetShaderResources(0, 1, nullSRV);
    if (cbuf) { cbuf->Release(); cbuf = nullptr; }

    // Second blur pass: vertical (to backbuffer)
    BindBackBuffer();

    // Bind source SRV: temp
    ctx->PSSetShaderResources(0, 1, &m_TempSRV);

    BlurCB cbV = {};
    cbV.Direction = XMFLOAT2(0.0f, 1.0f);
    cbV.Sigma = m_Sigma;
    cbV.Radius = m_Radius;
    cbV.TexelSize = XMFLOAT2(1.0f / SCREEN_WIDTH, 1.0f / SCREEN_HEIGHT);

    init.pSysMem = &cbV;
    if (SUCCEEDED(GetDevice()->CreateBuffer(&bd, &init, &cbuf)))
    {
        ctx->PSSetConstantBuffers(8, 1, &cbuf);
    }

    ctx->Draw(4, 0);

    // Cleanup
    ctx->PSSetShaderResources(0, 1, nullSRV);
    if (cbuf) { cbuf->Release(); }
}
