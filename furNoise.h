/*==============================================================================

   ファーノイズテクスチャ生成 [furNoise.h]

==============================================================================*/
#pragma once

#include "main.h"

// ランタイムでファー用ノイズテクスチャを生成し、SRV を返す
// width, height: テクスチャサイズ
// density: 白い点（毛の根元）の密度 (0.0?1.0)
ID3D11ShaderResourceView* CreateFurNoiseTexture(
    ID3D11Device* device,
    int width, int height,
    float density
);
