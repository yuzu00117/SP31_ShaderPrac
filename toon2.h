/*==============================================================================

   ランプテクスチャトゥーンシェーディング [toon2.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once

#include "main.h"
#include "renderer.h"
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

////////////////
//構造体
////////////////

class Toon2Model
{
protected:
	XMFLOAT3	Position;
	XMFLOAT3	Scale;
	XMFLOAT3	Rotate;
	int			TexID;         // カラーテクスチャID
	int			RampTexID;     // ランプテクスチャID

	MODEL* Model;

public:
	HRESULT InitPolygonModel(void);
	void FinalizePolygonModel(void);
	void UpdatePolygonModel(void);
	void DrawPolygonModel(void);
	
	// ランプテクスチャIDを取得する関数（新規追加）
	int GetRampTextureID(void) const { return RampTexID; }
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************