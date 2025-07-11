#include "skyDome.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"
#include "renderer.h"
#include <vector>

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT SkyDome::InitSkyDome(void)
{
    // シェーダー読み込み
    CreateVertexShader(&VertexShader, &VertexLayout, "skyDomeVS.cso");
    CreatePixelShader(&PixelShader, "skyDomePS.cso");

    // スカイドームのジオメトリを作成
    CreateSphereGeometry();

    // 3Dオブジェクト管理構造体の初期化
    Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Scale = XMFLOAT3(50.0f, 50.0f, 50.0f); // スケールを大きくして遠く見せる

    // テクスチャは使用しない（シェーダーでグラデーションを描画）
    TexID = -1;

    return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void SkyDome::FinalizeSkyDome(void)
{
    if (VertexBuffer) VertexBuffer->Release();
    if (IndexBuffer) IndexBuffer->Release();
    if (VertexShader) VertexShader->Release();
    if (PixelShader) PixelShader->Release();
    if (VertexLayout) VertexLayout->Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void SkyDome::UpdateSkyDome(void)
{
    // スカイドームは通常回転させたりしない
    // 必要に応じてここで回転処理を追加
}

//=============================================================================
// 描画処理
//=============================================================================
void SkyDome::DrawSkyDome(void)
{
    // 深度テストは有効にするが、深度書き込みを無効にする
    SetDepthEnable(true);
    
    // 深度書き込み無効のステートを作成
    ID3D11DepthStencilState* depthState;
    D3D11_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度書き込み無効
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthDesc.StencilEnable = FALSE;
    
    GetDevice()->CreateDepthStencilState(&depthDesc, &depthState);
    GetDeviceContext()->OMSetDepthStencilState(depthState, 0);

    // カリングを無効にする（球の内側を確実に見るため）
    ID3D11RasterizerState* rasterizerState;
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE; // カリング無効
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;

    GetDevice()->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    GetDeviceContext()->RSSetState(rasterizerState);

    // 頂点レイアウト設定
    GetDeviceContext()->IASetInputLayout(VertexLayout);
    // 頂点シェーダー設定
    GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
    // ピクセルシェーダー設定
    GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

    {
        // テクスチャは使用しない（シェーダーでグラデーション描画）

        // 平行移動行列作成（カメラに追従）
        XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
        // 回転行列作成
        XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(
            XMConvertToRadians(Rotate.x),
            XMConvertToRadians(Rotate.y),
            XMConvertToRadians(Rotate.z)
        );
        // スケーリング行列作成
        XMMATRIX ScalingMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
        // ワールド行列作成
        XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;
        // DirectXに設定
        SetWorldMatrix(WorldMatrix);

        // 頂点バッファ設定
        UINT stride = sizeof(VERTEX_3D);
        UINT offset = 0;
        GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
        GetDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        // プリミティブトポロジー設定
        GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // マテリアル設定
        MATERIAL material;
        ZeroMemory(&material, sizeof(MATERIAL));
        material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        SetMaterial(material);

        // 描画
        GetDeviceContext()->DrawIndexed(IndexCount, 0, 0);
    }

    // 状態を元に戻す
    if (rasterizerState) rasterizerState->Release();
    if (depthState) depthState->Release();
    
    // 通常の深度設定に戻す
    SetDepthEnable(true);
}

//=============================================================================
// 球体ジオメトリ作成
//=============================================================================
void SkyDome::CreateSphereGeometry(void)
{
    std::vector<VERTEX_3D> vertices;
    std::vector<UINT> indices;

    // 球体の頂点を生成
    for (int ring = 0; ring <= SKYDOME_RINGS; ring++)
    {
        float phi = XM_PI * ring / SKYDOME_RINGS;
        for (int segment = 0; segment <= SKYDOME_SEGMENTS; segment++)
        {
            float theta = 2.0f * XM_PI * segment / SKYDOME_SEGMENTS;

            VERTEX_3D vertex;
            vertex.Position.x = SKYDOME_RADIUS * sinf(phi) * cosf(theta);
            vertex.Position.y = SKYDOME_RADIUS * cosf(phi);
            vertex.Position.z = SKYDOME_RADIUS * sinf(phi) * sinf(theta);

            // 法線は中心から外向き
            vertex.Normal.x = vertex.Position.x / SKYDOME_RADIUS;
            vertex.Normal.y = vertex.Position.y / SKYDOME_RADIUS;
            vertex.Normal.z = vertex.Position.z / SKYDOME_RADIUS;

            vertex.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            
            // テクスチャ座標を適切に設定
            vertex.TexCoord.x = (float)segment / SKYDOME_SEGMENTS;
            vertex.TexCoord.y = (float)ring / SKYDOME_RINGS;

            vertices.push_back(vertex);
        }
    }

    // インデックスを生成（内側から見るために順序を逆にする）
    for (int ring = 0; ring < SKYDOME_RINGS; ring++)
    {
        for (int segment = 0; segment < SKYDOME_SEGMENTS; segment++)
        {
            int current = ring * (SKYDOME_SEGMENTS + 1) + segment;
            int next = current + SKYDOME_SEGMENTS + 1;

            // 三角形1（内側から見るために頂点順序を逆にする）
            indices.push_back(current);
            indices.push_back(current + 1);
            indices.push_back(next);

            // 三角形2
            indices.push_back(current + 1);
            indices.push_back(next + 1);
            indices.push_back(next);
        }
    }

    VertexCount = vertices.size();
    IndexCount = indices.size();

    // 頂点バッファ作成
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(VERTEX_3D) * VertexCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices.data();
    GetDevice()->CreateBuffer(&bd, &InitData, &VertexBuffer);

    // インデックスバッファ作成
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(UINT) * IndexCount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices.data();
    GetDevice()->CreateBuffer(&bd, &InitData, &IndexBuffer);
}