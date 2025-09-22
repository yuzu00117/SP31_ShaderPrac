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


class WaterField
{
protected:
	XMFLOAT3	Position;
	XMFLOAT3	Scale;
	XMFLOAT3	Rotate;
	int			TexID;
	int			NormalMapTexID; // 法線マップ用

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* VertexLayout;

public:
	HRESULT InitWaterField(void);
	void FinalizeWaterField(void);
	void UpdateWaterField(void);
	void DrawWaterField(void);

	// 位置・回転・スケール設定用の簡易API
	void SetPosition(const XMFLOAT3& pos) { Position = pos; }
	void SetRotate(const XMFLOAT3& rot) { Rotate = rot; }
	void SetScale(const XMFLOAT3& scl) { Scale = scl; }
};







//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
