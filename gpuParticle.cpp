/*==============================================================================

   GPUパーティクルシステム [gpuParticle.cpp]
                                                         Author :
                                                         Date   :
--------------------------------------------------------------------------------
   Compute Shader でパーティクル更新、
   Geometry Shader でビルボード展開して描画
==============================================================================*/

#include "main.h"
#include "renderer.h"
#include "gpuParticle.h"
#include <stdlib.h>
#include <time.h>
#include <io.h>

// =============================================================================
// 初期化
// =============================================================================
void GPUParticle::Init(int maxParticles)
{
    m_MaxParticles = maxParticles;

    ID3D11Device* device = GetDevice();

    // --- シェーダー読み込み ---
    // コンピュートシェーダー
    CreateComputeShader(&m_ComputeShader, "particleCS.cso");

    // 頂点シェーダー（InputLayout不要なので直接作成）
    {
        FILE* file;
        long int fsize;
        file = fopen("particleVS.cso", "rb");
        fsize = _filelength(_fileno(file));
        unsigned char* buffer = new unsigned char[fsize];
        fread(buffer, fsize, 1, file);
        fclose(file);

        device->CreateVertexShader(buffer, fsize, NULL, &m_VertexShader);
        delete[] buffer;
    }

    // ジオメトリシェーダー
    CreateGeometryShader(&m_GeometryShader, "particleGS.cso");

    // ピクセルシェーダー
    CreatePixelShader(&m_PixelShader, "particlePS.cso");

    // --- パーティクル初期データ作成 ---
    srand((unsigned int)time(NULL));
    Particle* initData = new Particle[m_MaxParticles];
    for (int i = 0; i < m_MaxParticles; i++)
    {
        initData[i].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
        initData[i].Life = 0.0f; // 最初は全て寿命切れ→即リスポーン
        initData[i].Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
        initData[i].MaxLife = 2.0f;
    }

    // --- StructuredBuffer 作成 ---
    D3D11_BUFFER_DESC bufDesc = {};
    bufDesc.ByteWidth = sizeof(Particle) * m_MaxParticles;
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufDesc.StructureByteStride = sizeof(Particle);

    D3D11_SUBRESOURCE_DATA initSubData = {};
    initSubData.pSysMem = initData;

    device->CreateBuffer(&bufDesc, &initSubData, &m_ParticleBuffer);
    delete[] initData;

    // --- UAV (Compute Shader 用) ---
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.NumElements = m_MaxParticles;
    device->CreateUnorderedAccessView(m_ParticleBuffer, &uavDesc, &m_ParticleUAV);

    // --- SRV (Vertex Shader 用) ---
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.NumElements = m_MaxParticles;
    device->CreateShaderResourceView(m_ParticleBuffer, &srvDesc, &m_ParticleSRV);

    // --- 定数バッファ（CS用パラメータ） ---
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(ParticleParams);
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    device->CreateBuffer(&cbDesc, NULL, &m_ParamsCB);

    // --- 加算合成用 BlendState ---
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE; // premultiplied alpha outputなのでONE
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE; // 加算
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    device->CreateBlendState(&blendDesc, &m_BlendState);

    // --- ラスタライザステート（カリングなし） ---
    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_NONE;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthClipEnable = TRUE;
    device->CreateRasterizerState(&rsDesc, &m_RasterizerState);

    // --- 深度書き込み無効のDepthStencilState ---
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度書き込みしない
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    device->CreateDepthStencilState(&dsDesc, &m_DepthState);
}

// =============================================================================
// 終了処理
// =============================================================================
void GPUParticle::Finalize()
{
    if (m_DepthState)     { m_DepthState->Release();     m_DepthState = NULL; }
    if (m_RasterizerState){ m_RasterizerState->Release();m_RasterizerState = NULL; }
    if (m_BlendState)     { m_BlendState->Release();     m_BlendState = NULL; }
    if (m_ParamsCB)       { m_ParamsCB->Release();       m_ParamsCB = NULL; }
    if (m_ParticleSRV)    { m_ParticleSRV->Release();    m_ParticleSRV = NULL; }
    if (m_ParticleUAV)    { m_ParticleUAV->Release();    m_ParticleUAV = NULL; }
    if (m_ParticleBuffer) { m_ParticleBuffer->Release(); m_ParticleBuffer = NULL; }
    if (m_PixelShader)    { m_PixelShader->Release();    m_PixelShader = NULL; }
    if (m_GeometryShader) { m_GeometryShader->Release(); m_GeometryShader = NULL; }
    if (m_VertexShader)   { m_VertexShader->Release();   m_VertexShader = NULL; }
    if (m_ComputeShader)  { m_ComputeShader->Release();  m_ComputeShader = NULL; }
}

// =============================================================================
// 更新（Compute Shaderでパーティクル演算）
// =============================================================================
void GPUParticle::Update(float deltaTime)
{
    ID3D11DeviceContext* ctx = GetDeviceContext();

    // 定数バッファ更新
    ParticleParams params;
    params.EmitterPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
    params.DeltaTime = deltaTime;
    // フレームごとに変化する乱数シード
    params.Seed = XMFLOAT4(
        (float)(rand() % 10000) / 10000.0f,
        (float)(rand() % 10000) / 10000.0f,
        (float)(rand() % 10000) / 10000.0f,
        (float)(rand() % 10000) / 10000.0f
    );
    ctx->UpdateSubresource(m_ParamsCB, 0, NULL, &params, 0, 0);

    // コンピュートシェーダー実行
    ctx->CSSetShader(m_ComputeShader, NULL, 0);
    ctx->CSSetConstantBuffers(11, 1, &m_ParamsCB);
    ctx->CSSetUnorderedAccessViews(0, 1, &m_ParticleUAV, NULL);

    UINT threadGroups = (m_MaxParticles + 255) / 256;
    ctx->Dispatch(threadGroups, 1, 1);

    // 後片付け（他のシェーダーに影響しないようにクリア）
    ID3D11UnorderedAccessView* nullUAV = NULL;
    ctx->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);
    ctx->CSSetShader(NULL, NULL, 0);
}

// =============================================================================
// 描画（GS + PS でビルボードパーティクルを描画）
// =============================================================================
void GPUParticle::Draw()
{
    ID3D11DeviceContext* ctx = GetDeviceContext();

    // --- 描画ステート保存 ---
    ID3D11BlendState* prevBlendState = NULL;
    FLOAT prevBlendFactor[4];
    UINT prevSampleMask;
    ctx->OMGetBlendState(&prevBlendState, prevBlendFactor, &prevSampleMask);

    ID3D11DepthStencilState* prevDepthState = NULL;
    UINT prevStencilRef;
    ctx->OMGetDepthStencilState(&prevDepthState, &prevStencilRef);

    ID3D11RasterizerState* prevRS = NULL;
    ctx->RSGetState(&prevRS);

    // --- 加算合成・深度書き込み無効を設定 ---
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    ctx->OMSetBlendState(m_BlendState, blendFactor, 0xFFFFFFFF);
    ctx->OMSetDepthStencilState(m_DepthState, 0);
    ctx->RSSetState(m_RasterizerState);

    // --- シェーダー設定 ---
    ctx->VSSetShader(m_VertexShader, NULL, 0);
    ctx->GSSetShader(m_GeometryShader, NULL, 0);
    ctx->PSSetShader(m_PixelShader, NULL, 0);

    // SRV を頂点シェーダーにバインド
    ctx->VSSetShaderResources(0, 1, &m_ParticleSRV);

    // IA設定（頂点バッファなし、ポイントリスト）
    ctx->IASetInputLayout(NULL);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    UINT stride = 0;
    UINT offset = 0;
    ID3D11Buffer* nullBuffer = NULL;
    ctx->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);

    // --- 描画 ---
    ctx->Draw(m_MaxParticles, 0);

    // --- 後片付け ---
    // SRV クリア
    ID3D11ShaderResourceView* nullSRV = NULL;
    ctx->VSSetShaderResources(0, 1, &nullSRV);

    // ジオメトリシェーダー無効化
    ctx->GSSetShader(NULL, NULL, 0);

    // --- 描画ステート復帰 ---
    ctx->OMSetBlendState(prevBlendState, prevBlendFactor, prevSampleMask);
    if (prevBlendState) prevBlendState->Release();

    ctx->OMSetDepthStencilState(prevDepthState, prevStencilRef);
    if (prevDepthState) prevDepthState->Release();

    ctx->RSSetState(prevRS);
    if (prevRS) prevRS->Release();
}
