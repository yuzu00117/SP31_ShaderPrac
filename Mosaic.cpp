#include "Mosaic.h"
#include "keyboard.h"
#include <d3dcompiler.h>

struct FSVertex
{
    XMFLOAT3 pos;
    XMFLOAT2 uv;
};

Mosaic::Mosaic() {}
Mosaic::~Mosaic() { Finalize(); }

HRESULT Mosaic::Initialize()
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

void Mosaic::Finalize()
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

HRESULT Mosaic::CreateOffscreenTargets()
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

HRESULT Mosaic::CreateFullScreenQuad()
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

HRESULT Mosaic::CreateShaders()
{
    // Compile HLSL at runtime and create shaders + input layout (POSITION, TEXCOORD)
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(L"shader/mosaicVS.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, &errBlob);
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

    hr = D3DCompileFromFile(L"shader/mosaicPS.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob) { OutputDebugStringA((const char*)errBlob->GetBufferPointer()); errBlob->Release(); }
        return hr;
    }

    hr = GetDevice()->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_PS);
    psBlob->Release();
    return hr;
}

void Mosaic::SetRectSize(int size)
{
    if (size < 1) size = 1;
    if (size > 512) size = 512;
    m_RectSize = size;
}

void Mosaic::Update()
{
    // Toggle with V key (trigger)
    if (Keyboard_IsKeyDownTrigger(KK_V))
    {
        m_Enabled = !m_Enabled;
    }

    // Handle UP/DOWN keys for size
    if (Keyboard_IsKeyDownTrigger(KK_UP))
    {
        SetRectSize(m_RectSize + 1);
    }
    if (Keyboard_IsKeyDownTrigger(KK_DOWN))
    {
        SetRectSize(m_RectSize - 1);
    }
}

void Mosaic::BeginScene()
{
    auto ctx = GetDeviceContext();
    // Bind offscreen RTV + DSV
    ctx->OMSetRenderTargets(1, &m_OffscreenRTV, m_OffscreenDSV);

    // Clear
    float clear[4] = {0,0,0,1};
    ctx->ClearRenderTargetView(m_OffscreenRTV, clear);
    ctx->ClearDepthStencilView(m_OffscreenDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Mosaic::EndSceneAndDraw()
{
    auto ctx = GetDeviceContext();

    // Bind backbuffer RTV/DSV
    BindBackBuffer();

    if (!m_Enabled)
    {
        // When disabled, just blit the offscreen texture 1:1 without mosaic.
        // Reuse the same full-screen pass but with RectSize = 1.
        SetDepthEnable(false);
        ctx->IASetInputLayout(m_InputLayout);
        ctx->VSSetShader(m_VS, nullptr, 0);
        ctx->PSSetShader(m_PS, nullptr, 0);

        UINT stride = sizeof(FSVertex);
        UINT offset = 0;
        ctx->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
        ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        ctx->PSSetShaderResources(0, 1, &m_OffscreenSRV);

        XMFLOAT4 param((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 1.0f, 0.0f);
        SetParameter(param);

        ctx->Draw(4, 0);

        ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
        ctx->PSSetShaderResources(0, 1, nullSRV);
        return;
    }

    // Back buffer resolve pass: depth off
    SetDepthEnable(false);

    // Set shaders and input layout for fullscreen pass
    ctx->IASetInputLayout(m_InputLayout);
    ctx->VSSetShader(m_VS, nullptr, 0);
    ctx->PSSetShader(m_PS, nullptr, 0);

    // Fullscreen quad VB
    UINT stride = sizeof(FSVertex);
    UINT offset = 0;
    ctx->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // Bind SRV from first pass
    ctx->PSSetShaderResources(0, 1, &m_OffscreenSRV);

    // Set Parameter = {ScreenW, ScreenH, RectSize, 0}
    XMFLOAT4 param((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, (float)m_RectSize, 0.0f);
    SetParameter(param);

    // Draw 4 verts
    ctx->Draw(4, 0);

    // Unbind SRV to avoid binding conflict next frame
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    ctx->PSSetShaderResources(0, 1, nullSRV);
}
