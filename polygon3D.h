/*==============================================================================

   頂点管理[polygon3D.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once


#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


////////////////
//構造体
////////////////


class Object3D
{
	protected:
		XMFLOAT3	Position;
		XMFLOAT3	Scale;
		XMFLOAT3	Rotate;
		int			TexID;


		ID3D11Buffer* VertexBuffer;	//テスト用頂点バッファ
		ID3D11Buffer* IndexBuffer;	//テスト用インデックスバッファ


		ID3D11VertexShader* VertexShader;
		ID3D11PixelShader* PixelShader;
		ID3D11InputLayout* VertexLayout;

	public:
		HRESULT InitPolygon3D(void);
		void FinalizePolygon3D(void);
		void UpdatePolygon3D(void);
		void DrawPolygon3D(void);



};







//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
