/*==============================================================================

   頂点管理 [ProjectionShadow.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include"ProjectionShadow.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"
#include "model.h"
#include"renderer.h"



//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
//カメラは複数あるとダメなので外す

static LIGHT Light;

static ID3D11VertexShader* g_VertexShader[2] = { NULL, NULL };
static ID3D11InputLayout* g_VertexLayout = NULL;
static ID3D11PixelShader* g_PixelShader[2] = { NULL, NULL };

//初期化時


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT ProjectionShadow::InitPolygonModel(void)
{

	    // シェーダー読み込み
    CreateVertexShader(&g_VertexShader[0], &g_VertexLayout, "pixelLightingBlinnPhongVS.cso");
    CreatePixelShader(&g_PixelShader[0], "pixelLightingBlinnPhongPS.cso");

    CreateVertexShader(&g_VertexShader[1], &g_VertexLayout, "ProjectionShadow0VS.cso");
    CreatePixelShader(&g_PixelShader[1], "ProjectionShadow0PS.cso");

	    // ライトパラメーター（フィールドと数値を合わせて）
    Light.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
    Light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

    XMVECTOR temp = XMVectorSet(1.0f, -1.0f, 0.0f, 0.0f);
    temp = XMVector3Normalize(temp);
    XMStoreFloat4(&Light.Direction, temp);

    Light.Position       = XMFLOAT4(0.0f, 2.0f, -1.0f, 0.0f);
    Light.PointLightParam = XMFLOAT4(1000.0f, 5.0f, 0.5f, 0.5f);

	//3Dオブジェクト管理構造体の初期化
	Position = XMFLOAT3(0.0f, 0.2f, 1.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);


	//モデル読み込み
	Model = ModelLoad("asset\\model\\model.fbx");




	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void ProjectionShadow::FinalizePolygonModel(void)
{
	//作ったものを解放
	g_VertexShader[0]->Release(); g_VertexShader[0] = NULL;
	g_VertexShader[1]->Release(); g_VertexShader[1] = NULL;
	g_PixelShader[0]->Release(); g_PixelShader[0] 	= NULL;
	g_PixelShader[1]->Release(); g_PixelShader[1] 	= NULL;
	g_VertexLayout->Release();    g_VertexLayout    = NULL;

	ModelRelease(Model);

}

//=============================================================================
// 更新処理
//=============================================================================
void ProjectionShadow::UpdatePolygonModel(void)
{
	//適当に回転
	Rotate.y += 0.3f;
	Rotate.x += 0.1f;

}

//=============================================================================
// 描画処理
//=============================================================================
void ProjectionShadow::DrawPolygonModel(int n)
{
    // ライトカメラのビュー行列
    XMVECTOR lpos = XMVectorSet(Light.Position.x, Light.Position.y, Light.Position.z, 0.0f);
    XMVECTOR lat  = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR lup  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    Light.ViewMatrix = XMMatrixLookAtLH(lpos, lat, lup);

    // ライトカメラのプロジェクション行列
    Light.ProjectionMatrix = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        (float)SCREEN_WIDTH / SCREEN_WIDTH, // アスペクト比 1.0f
        1.0f,
        1000.0f
    );

    SetLight(Light); // ライトをシェーダへ送る

    //---------------------------------------
    // 頂点レイアウトを設定
    GetDeviceContext()->IASetInputLayout(g_VertexLayout);
    // 頂点シェーダーをセット（n で 0/1 を切り替え）
    GetDeviceContext()->VSSetShader(g_VertexShader[n], NULL, 0);
    // ピクセルシェーダーをセット
    GetDeviceContext()->PSSetShader(g_PixelShader[n], NULL, 0);

    // シャドウマップ作成時は描画用行列をライト用に書き換える
    if (n == 1)
    {
        SetViewMatrix(Light.ViewMatrix);
        SetProjectionMatrix(Light.ProjectionMatrix);
    }

    //---- ここからは元の描画処理をそのまま ----

    // 3D用行列設定
    SetDepthEnable(true);		// 奥行き処理有効

    {   // 3Dポリゴン１つずつの処理
        // テクスチャをセット
        ID3D11ShaderResourceView* tex = GetTexture(TexID);
        GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

        // 平行移動行列
        XMMATRIX TranslationMatrix =
            XMMatrixTranslation(
                Position.x,
                Position.y,
                Position.z
            );
        // 回転行列
        XMMATRIX RotationMatrix =
            XMMatrixRotationRollPitchYaw(
                XMConvertToRadians(Rotate.x),
                XMConvertToRadians(Rotate.y),
                XMConvertToRadians(Rotate.z)
            );
        // スケーリング行列
        XMMATRIX ScalingMatrix =
            XMMatrixScaling(
                Scale.x,
                Scale.y,
                Scale.z
            );
        // ワールド行列（乗算順に注意）
        XMMATRIX WorldMatrix =
            ScalingMatrix *
            RotationMatrix *
            TranslationMatrix;

        SetWorldMatrix(WorldMatrix);

        // プリミティブトポロジー
        GetDeviceContext()->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );

        // マテリアル設定
        MATERIAL material;
        ZeroMemory(&material, sizeof(MATERIAL));
        material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        SetMaterial(material);

        // 描画
        ModelDraw(Model);
    }
}