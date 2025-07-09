/*==============================================================================

   頂点管理 [bumpMapField.cpp]
								 Author :
								 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "bumpMapField.h"
#include "sprite.h"

#include "texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	BOX_NUM_VERTEX	(4)
#define	SIZE	(5)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static LIGHT Light;



//*****************************************************************************
// グローバル変数
//*****************************************************************************

//頂点データ
/*
	  V0------------------V1
	  |                  |
	 |                  |
	|                  |
   V2-----------------V3
*/

VERTEX_3D	Bump_Box[BOX_NUM_VERTEX] =
{
	//天井////////////////////////////////////////////
	{//頂点V0 LEFT-TOP
		XMFLOAT3(-SIZE / 2, 0.0f, SIZE / 2),	//頂点座標
		XMFLOAT3(0.0f,1.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(0.0f * SIZE,0.0f * SIZE)				//テクスチャ座標
	},
	{//頂点V1 RIGHT-TOP
		XMFLOAT3(SIZE / 2, 0.0f, SIZE / 2),	//頂点座標
		XMFLOAT3(0.0f,1.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(1.0f * SIZE,0.0f * SIZE)				//テクスチャ座標
	},
	{//頂点V2 LEFT-BOTTOM
		XMFLOAT3(-SIZE / 2, 0.0f, -SIZE / 2),	//頂点座標
		XMFLOAT3(0.0f,1.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(0.0f * SIZE,1.0f * SIZE)				//テクスチャ座標
	},

	{//頂点V3 RIGHT-BOTTOM
		XMFLOAT3(SIZE / 2, 0.0f, -SIZE / 2),		//頂点座標
		XMFLOAT3(0.0f,1.0f,0.0f),		//法線
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//カラー
		XMFLOAT2(1.0f * SIZE,1.0f * SIZE)				//テクスチャ座標
	},

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT BumpMapField::InitBumpMapField(void)
{

	//テクスチャ読み込み
	int Texture = TextureLoad(L"asset\\texture\\sura.jpg");
	BumpTexID = TextureLoad(L"asset\\texture\\test.png");

	// シェーダーの読み込み
	CreateVertexShader(&VertexShader, &VertexLayout, "bumpVS.cso");
	CreatePixelShader(&PixelShader, "bumpPS.cso");

	//ライトパラメータを設定
	Light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	Light.Ambient = XMFLOAT4(0.5f, 0.3f, 0.3f, 1.0f);
	Light.Direction = XMFLOAT4(0.0f, -1.0f, 1.0f, 0.0f);

	Light.Position = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);//モデルの少し手前少し上
	Light.PointLightParam = XMFLOAT4(10.0f, 0.0f, 0.0f, 0.0f);//x=距離
	Light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);//アンビエントもいれる

	//CreateVertexShader(&VertexShader, &VertexLayout, "UnlitTextureVS.cso");
	//CreatePixelShader(&PixelShader, "UnlitTexturePS.cso");
	//頂点バッファ作成
	{
		D3D11_BUFFER_DESC	bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * BOX_NUM_VERTEX;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		GetDevice()->CreateBuffer(&bd, NULL, &VertexBuffer);

		//頂点バッファの書き込み先のポインターを取得
		D3D11_MAPPED_SUBRESOURCE	msr;
		GetDeviceContext()->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		//頂点データをコピー
		CopyMemory(&vertex[0], &Bump_Box[0], sizeof(VERTEX_3D) * BOX_NUM_VERTEX);
		//書き込み完了
		GetDeviceContext()->Unmap(VertexBuffer, 0);
	}

	//3Dオブジェクト管理構造体の初期化
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	TexID = Texture;

	////ライトパラメーター
	//Light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	//Light.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);

	//Light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f); //ライトの向き
	//Light.Position = XMFLOAT4(0.0f, 2.0f, 0.0f, 0.0f);   //ライトの座標
	//Light.PointLightParam = XMFLOAT4(200.0f, 1.5f, 0.0f, 0.0f); //x = 光の届く距離 y = 補正値
	//Light.Angle = XMFLOAT4((3.14159f / 180.0f) * 30.0f, 0.0f, 0.0f, 0.0f); //コーン角度

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void BumpMapField::FinalizeBumpMapField(void)
{
	//作ったものを解放
	VertexBuffer->Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void BumpMapField::UpdateBumpMapField(void)
{
	if (GetAsyncKeyState('E') & 0x8000)
	{
		Light.Position.x += 0.1f;
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		Light.Position.x -= 0.1f;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void BumpMapField::DrawBumpMapField(void)
{
	SetLight(Light);

	// 頂点レイアウト設定
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	{
		//テクスチャをセット
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		// バンプマップテクスチャをセット
		ID3D11ShaderResourceView* bumpTex = GetTexture(BumpTexID);
		GetDeviceContext()->PSSetShaderResources(1, 1, &bumpTex); // register(t1) にバインド

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

		//頂点バッファをセット
		UINT	stride = sizeof(VERTEX_3D);
		UINT	offset = 0;
		GetDeviceContext()->IASetVertexBuffers(
			0,
			1,
			&VertexBuffer,
			&stride,
			&offset
		);

		//プリミティブトポロジーの設定
		GetDeviceContext()->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		);
		//マテリアル設定
		MATERIAL	material;
		ZeroMemory(&material, sizeof(MATERIAL));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//描画
		GetDeviceContext()->Draw(BOX_NUM_VERTEX, 0);//インデックス無し描画
	}
}
