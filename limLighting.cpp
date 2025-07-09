/*==============================================================================

   頂点管理 [polygon3D.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include"limLighting.h"
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

static ID3D11VertexShader* g_VertexShader = NULL;
static ID3D11InputLayout* g_VertexLayout = NULL;
static ID3D11PixelShader* g_PixelShader = NULL;

//初期化時


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT LimLightingModel::InitPolygonModel(void)
{

	//シェーダー読み込み
	CreateVertexShader(&g_VertexShader, &g_VertexLayout, "limLightingVS.cso");
	CreatePixelShader(&g_PixelShader, "limLightingPS.cso");

	//ライトパラメータを設定


	Light.Direction = XMFLOAT4(-0.5f, -1.0f, 0.5f, 0.0f);
	Light.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	Light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f); // 真上から光を当てる（Rimが出やすい）



	//3Dオブジェクト管理構造体の初期化
	Position = XMFLOAT3(-0.1f, 0.2f, 0.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);


	//モデル読み込み
	Model = ModelLoad("asset\\model\\model.fbx");




	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void LimLightingModel::FinalizePolygonModel(void)
{
	//作ったものを解放



	ModelRelease(Model);

}

//=============================================================================
// 更新処理
//=============================================================================
void LimLightingModel::UpdatePolygonModel(void)
{
	//適当に回転
	Rotate.y += 1.3f;
	Rotate.x += 0.1f;

}

//=============================================================================
// 描画処理
//=============================================================================
void LimLightingModel::DrawPolygonModel(void)
{
	SetLight(Light);


	//---------------------------------------
	//頂点レイアウトを設定
	GetDeviceContext()->IASetInputLayout(g_VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(g_VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(g_PixelShader, NULL, 0);

	//3D用行列設定
	//ResetWorldViewProjection3D();//3D描画に切り替える
	SetDepthEnable(true);		//奥行き処理有効

	//カメラは共有の情報が設定されているとする

	{//3Dポリゴン１つずつの処理
		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//平行移動行列作成
		XMMATRIX	TranslationMatrix =
			XMMatrixTranslation(
				Position.x,
				Position.y,
				Position.z
			);
		//回転行列作成
		XMMATRIX	RotationMatrix =
			XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(Rotate.x),
				XMConvertToRadians(Rotate.y),
				XMConvertToRadians(Rotate.z)
			);
		//スケーリング行列作成
		XMMATRIX	ScalingMatrix =
			XMMatrixScaling(
				Scale.x,
				Scale.y,
				Scale.z
			);
		//ワールド行列作成 ※乗算の順番に注意
		XMMATRIX	WorldMatrix =
			ScalingMatrix *
			RotationMatrix *
			TranslationMatrix;
		//DirectXへセット
		SetWorldMatrix(WorldMatrix);

		//プリミティブトポロジーの設定
		GetDeviceContext()->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);
		//マテリアル設定
		MATERIAL	material;
		ZeroMemory(&material, sizeof(MATERIAL));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//描画
		ModelDraw(Model);
	}


}
