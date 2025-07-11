/*==============================================================================

簡易スプライト表示
  sprite.h
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


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSprite(void);
void FinalizeSprite(void);
//スプライト表示　行列使用版
void DrawSprite(XMFLOAT2 size, XMFLOAT4 color);
