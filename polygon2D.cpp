/*==============================================================================

   頂点管理 [polygon2D.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "polygon2D.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"

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
HRESULT Object2D::InitPolygon2D(void)
{
	// テクスチャ読み込み
	int Texture = TextureLoad(L"asset\\texture\\center_white.png");
	TexID = Texture;

	//シェーダー読み込み
	CreateVertexShader(&VertexShader, &VertexLayout, "UnlitTextureVS.cso");
	CreatePixelShader(&PixelShader, "UnlitTexturePS.cso");


	//2Dオブジェクト初期化
	Position = XMFLOAT3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Scale = XMFLOAT2(0.01f, 0.01f);
	Size = XMFLOAT2(SCREEN_WIDTH / 3, SCREEN_WIDTH / 3);
	Rotate = 0.0f;


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Object2D::FinalizePolygon2D(void)
{
	VertexShader->Release();
	PixelShader->Release();
	VertexLayout->Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void Object2D::UpdatePolygon2D(void)
{
	//適当に回転
	//Rotate += 0.2f;

}

//=============================================================================
// 描画処理
//=============================================================================
void Object2D::DrawPolygon2D(void)
{

	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	//奥行き処理をOFF
	SetDepthEnable(false);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	{//2Dポリゴン1枚ずつで必要な処理

		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//平行移動行列の作成（表示座標を決める）
		XMMATRIX	TranslationMatrix = XMMatrixTranslation(
			Position.x, Position.y, 0.0f);

		//回転行列（Z回転）行列の作成
		XMMATRIX	RotationMatrix = XMMatrixRotationZ(XMConvertToRadians(Rotate));

		//スケーリング行列作成（倍率1.0が等倍、0倍はダメ！）
		XMMATRIX	ScalingMatrix = XMMatrixScaling(Scale.x,Scale.y, 1.0f);

		//ワールド行列の作成（ポリゴンの表示の仕方を指定する最終的な行列
		XMMATRIX	WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

		//ワールド行列をDirectXへセット
		SetWorldMatrix(WorldMatrix);

		// ポリゴン描画
		DrawSprite(Size, Color);
	}


}
