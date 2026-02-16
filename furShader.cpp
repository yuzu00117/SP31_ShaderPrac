/*==============================================================================

   ファーシェーダー [furShader.cpp]
                                        Author :
                                        Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "main.h"
#include "renderer.h"
#include "furShader.h"
#include "furNoise.h"
#include "texture.h"
#include "Camera.h"


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT FurShaderModel::InitPolygonModel()
{
    // シェーダー読み込み
    CreateVertexShader(&g_VertexShader, &g_VertexLayout, "furVS.cso");
    CreatePixelShader(&g_PixelShader, "furPS.cso");

    // ライトパラメータを設定
    Light.Diffuse   = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    Light.Ambient   = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    Light.Direction = XMFLOAT4(0.0f, -1.0f, 1.0f, 0.0f);
    Light.Position  = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
    Light.PointLightParam = XMFLOAT4(10.0f, 0.0f, 0.0f, 0.0f);

    // 3Dオブジェクト管理構造体の初期化
    Position = XMFLOAT3(0.0f, 0.2f, 1.0f);
    Rotate   = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Scale    = XMFLOAT3(0.2f, 0.2f, 0.2f);

    // ファーパラメータ初期値
    FurLength  = 0.15f;   // 毛の長さ
    ShellCount = 32;       // シェル層数
    FurDensity = 0.5f;     // 毛の密度

    // モデル読み込み
    Model = ModelLoad("asset\\model\\model.fbx");

    // ノイズテクスチャ生成（128x128, 密度50%）
    g_NoiseSRV = CreateFurNoiseTexture(GetDevice(), 128, 128, FurDensity);

    // ファー専用定数バッファ作成 (b7)
    D3D11_BUFFER_DESC bufDesc = {};
    bufDesc.Usage          = D3D11_USAGE_DEFAULT;
    bufDesc.ByteWidth      = sizeof(FUR_PARAM);
    bufDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    bufDesc.CPUAccessFlags = 0;
    GetDevice()->CreateBuffer(&bufDesc, NULL, &g_FurParamBuffer);

    // αブレンドステート作成（有効）
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable           = TRUE;
    blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha         = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    GetDevice()->CreateBlendState(&blendDesc, &g_AlphaBlendState);

    // αブレンドステート作成（無効）
    blendDesc.RenderTarget[0].BlendEnable = FALSE;
    GetDevice()->CreateBlendState(&blendDesc, &g_NoBlendState);

    // 両面描画用ラスタライザステート
    D3D11_RASTERIZER_DESC rasDesc = {};
    rasDesc.FillMode = D3D11_FILL_SOLID;
    rasDesc.CullMode = D3D11_CULL_NONE;  // カリング無効 → 両面描画
    rasDesc.DepthClipEnable = TRUE;
    GetDevice()->CreateRasterizerState(&rasDesc, &g_NoCullState);

    return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void FurShaderModel::FinalizePolygonModel()
{
    if (g_NoCullState)    { g_NoCullState->Release();    g_NoCullState = nullptr; }
    if (g_NoBlendState)   { g_NoBlendState->Release();   g_NoBlendState = nullptr; }
    if (g_AlphaBlendState){ g_AlphaBlendState->Release(); g_AlphaBlendState = nullptr; }
    if (g_FurParamBuffer) { g_FurParamBuffer->Release();  g_FurParamBuffer = nullptr; }
    if (g_NoiseSRV)       { g_NoiseSRV->Release();        g_NoiseSRV = nullptr; }
    if (g_PixelShader)    { g_PixelShader->Release();     g_PixelShader = nullptr; }
    if (g_VertexLayout)   { g_VertexLayout->Release();    g_VertexLayout = nullptr; }
    if (g_VertexShader)   { g_VertexShader->Release();    g_VertexShader = nullptr; }

    if (Model) { ModelRelease(Model); Model = nullptr; }
}


//=============================================================================
// 更新処理
//=============================================================================
void FurShaderModel::UpdatePolygonModel()
{
    // 適当に回転
    Rotate.y += 0.3f;
    Rotate.x += 0.1f;
}


//=============================================================================
// 描画処理
//=============================================================================
void FurShaderModel::DrawPolygonModel()
{
    SetLight(Light);

    // シェーダー設定
    GetDeviceContext()->IASetInputLayout(g_VertexLayout);
    GetDeviceContext()->VSSetShader(g_VertexShader, NULL, 0);
    GetDeviceContext()->PSSetShader(g_PixelShader, NULL, 0);

    SetDepthEnable(true);

    // テクスチャをセット (t0 = モデルテクスチャ, t1 = ノイズテクスチャ)
    ID3D11ShaderResourceView* tex = GetTexture(TexID);
    GetDeviceContext()->PSSetShaderResources(0, 1, &tex);
    GetDeviceContext()->PSSetShaderResources(1, 1, &g_NoiseSRV);

    // ファー定数バッファを VS/PS の b7 にバインド
    GetDeviceContext()->VSSetConstantBuffers(7, 1, &g_FurParamBuffer);
    GetDeviceContext()->PSSetConstantBuffers(7, 1, &g_FurParamBuffer);

    // ワールド行列の計算
    XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
    XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(
        XMConvertToRadians(Rotate.x),
        XMConvertToRadians(Rotate.y),
        XMConvertToRadians(Rotate.z)
    );
    XMMATRIX ScalingMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
    XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;
    SetWorldMatrix(WorldMatrix);

    // プリミティブトポロジー設定
    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // マテリアル設定
    MATERIAL material;
    ZeroMemory(&material, sizeof(MATERIAL));
    material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    SetMaterial(material);

    // --- ベースメッシュの描画（不透明、シェル0相当） ---
    {
        FUR_PARAM param = {};
        param.FurLength   = 0.0f;  // 押し出しなし
        param.ShellIndex  = 0.0f;
        param.TotalShells = (float)ShellCount;
        param.FurDensity  = FurDensity;
        param.FurColor    = XMFLOAT4(0.6f, 0.45f, 0.3f, 1.0f); // 茶色系
        param.GravityDir  = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.1f);
        GetDeviceContext()->UpdateSubresource(g_FurParamBuffer, 0, NULL, &param, 0, 0);

        ModelDraw(Model);
    }

    // --- シェル層の描画（αブレンド有効、両面描画） ---
    // 現在のラスタライザステートを保存
    ID3D11RasterizerState* prevRasState = nullptr;
    GetDeviceContext()->RSGetState(&prevRasState);

    // 両面描画に切り替え
    GetDeviceContext()->RSSetState(g_NoCullState);

    // αブレンド有効化
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    GetDeviceContext()->OMSetBlendState(g_AlphaBlendState, blendFactor, 0xffffffff);

    for (int i = 1; i <= ShellCount; i++)
    {
        FUR_PARAM param = {};
        param.FurLength   = FurLength;
        param.ShellIndex  = (float)i;
        param.TotalShells = (float)ShellCount;
        param.FurDensity  = FurDensity;
        param.FurColor    = XMFLOAT4(0.6f, 0.45f, 0.3f, 1.0f); // 茶色系の毛
        param.GravityDir  = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.1f); // 下方向に軽い重力

        GetDeviceContext()->UpdateSubresource(g_FurParamBuffer, 0, NULL, &param, 0, 0);

        ModelDraw(Model);
    }

    // ブレンドステート復元（無効に戻す）
    GetDeviceContext()->OMSetBlendState(g_NoBlendState, blendFactor, 0xffffffff);

    // ラスタライザステート復元
    GetDeviceContext()->RSSetState(prevRasState);
    if (prevRasState) prevRasState->Release();
}
