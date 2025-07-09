///*==============================================================================
//
//   頂点管理 [polygon3D.cpp]
//														 Author :
//														 Date   :
//--------------------------------------------------------------------------------
//
//==============================================================================*/
//#include "polygon3D.h"
//#include "sprite.h"
//
//#include "texture.h"
//#include"bumpMap.h"
////*****************************************************************************
//// マクロ定義
////*****************************************************************************
//
//
//
////*****************************************************************************
//// プロトタイプ宣言
////*****************************************************************************
//
//
////*****************************************************************************
//// グローバル変数
////*****************************************************************************
//
////頂点データ
///*
//      V0------------------V1
//      |                  |
//     |                  |
//    |                  |
//   V2-----------------V3
//*/
//
//
//
//
////=============================================================================
//// 初期化処理
////=============================================================================
//HRESULT BumpMapField::InitPolygon3D(void)
//{
//
//	//テクスチャ読み込み
//	int Texture = TextureLoad(L"asset\\texture\\Normal.bmp");
//
//
//	//シェーダー読み込み
//	CreateVertexShader(&VertexShader, &VertexLayout, "UnlitTextureVS.cso");
//	CreatePixelShader(&PixelShader, "UnlitTexturePS.cso");
//	//頂点バッファ作成
//	{
//		D3D11_BUFFER_DESC	bd;
//		ZeroMemory(&bd, sizeof(bd));
//		bd.Usage = D3D11_USAGE_DYNAMIC;
//		bd.ByteWidth = sizeof(VERTEX_3D) * BOX_NUM_VERTEX;
//		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
//		GetDevice()->CreateBuffer(&bd, NULL, &VertexBuffer);
//
//		//頂点バッファの書き込み先のポインターを取得
//		D3D11_MAPPED_SUBRESOURCE	msr;
//		GetDeviceContext()->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
//		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
//
//		//頂点データをコピー
//		CopyMemory(&vertex[0], &Box[0], sizeof(VERTEX_3D) * BOX_NUM_VERTEX);
//		//書き込み完了
//		GetDeviceContext()->Unmap(VertexBuffer, 0);
//	}
//
//	//3Dオブジェクト管理構造体の初期化
//	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
//	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
//	Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
//	TexID = Texture;
//
//
//	return S_OK;
//}
//
////=============================================================================
//// 終了処理
////=============================================================================
//void BumpMapField::FinalizePolygon3D(void)
//{
//	//作ったものを解放
//
//
//
//	VertexBuffer->Release();
//}
//
////=============================================================================
//// 更新処理
////=============================================================================
//void BumpMapField::UpdatePolygon3D(void)
//{
//
//}
//
////=============================================================================
//// 描画処理
////=============================================================================
//void BumpMapField::DrawPolygon3D(void)
//{
//	// 頂点レイアウト設定
//	GetDeviceContext()->IASetInputLayout(VertexLayout);
//	//頂点シェーダーをセット
//	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
//	//ピクセルシェーダーをセット
//	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);
//
//	{
//		//テクスチャをセット
//		ID3D11ShaderResourceView* tex = GetTexture(TexID);
//		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);
//
//		//平行移動行列作成
//		XMMATRIX	TranslationMatrix =
//			XMMatrixTranslation(
//				Position.x,
//				Position.y,
//				Position.z
//			);
//		//回転行列作成
//		XMMATRIX	RotationMatrix =
//			XMMatrixRotationRollPitchYaw(
//				XMConvertToRadians(Rotate.x),
//				XMConvertToRadians(Rotate.y),
//				XMConvertToRadians(Rotate.z)
//			);
//		//スケーリング行列作成
//		XMMATRIX	ScalingMatrix =
//			XMMatrixScaling(
//				Scale.x,
//				Scale.y,
//				Scale.z
//			);
//		//ワールド行列作成 ※乗算の順番に注意
//		XMMATRIX	WorldMatrix =
//			ScalingMatrix *
//			RotationMatrix *
//			TranslationMatrix;
//		//DirectXへセット
//		SetWorldMatrix(WorldMatrix);
//
//		//頂点バッファをセット
//		UINT	stride = sizeof(VERTEX_3D);
//		UINT	offset = 0;
//		GetDeviceContext()->IASetVertexBuffers(
//			0,
//			1,
//			&VertexBuffer,
//			&stride,
//			&offset
//		);
//
//		//プリミティブトポロジーの設定
//		GetDeviceContext()->IASetPrimitiveTopology(
//			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
//		);
//		//マテリアル設定
//		MATERIAL	material;
//		ZeroMemory(&material, sizeof(MATERIAL));
//		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//		SetMaterial(material);
//
//		//描画
//		GetDeviceContext()->Draw(BOX_NUM_VERTEX, 0);//インデックス無し描画
//	}
//
//
//}
