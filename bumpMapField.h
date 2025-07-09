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


class BumpMapField
{
protected:
	XMFLOAT3	Position;
	XMFLOAT3	Scale;
	XMFLOAT3	Rotate;
	int			TexID;
	int			BumpTexID; // 法線マップ用

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* VertexLayout;

public:
	HRESULT InitBumpMapField(void);
	void FinalizeBumpMapField(void);
	void UpdateBumpMapField(void);
	void DrawBumpMapField(void);
};







//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
