/*==============================================================================

   ファーシェーダー [furShader.h]
                                        Author :
                                        Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once

#include "main.h"
#include "renderer.h"
#include "model.h"

//*****************************************************************************
// ファーシェーダー用定数バッファ構造体（GPU 送信用）
//*****************************************************************************
struct FUR_PARAM
{
    float  FurLength;       // 毛の長さ
    float  ShellIndex;      // 現在のシェル層
    float  TotalShells;     // 総シェル数
    float  FurDensity;      // 毛の密度

    XMFLOAT4 FurColor;     // 毛の色 (rgb) + α倍率 (a)
    XMFLOAT4 GravityDir;   // 重力方向 (xyz) + 強さ (w)
};

//*****************************************************************************
// ファーシェーダーモデルクラス
//*****************************************************************************
class FurShaderModel
{
protected:
    XMFLOAT3    Position;
    XMFLOAT3    Scale;
    XMFLOAT3    Rotate;
    int         TexID;

    MODEL* Model;

    // シェーダーリソース
    ID3D11VertexShader*  g_VertexShader;
    ID3D11PixelShader*   g_PixelShader;
    ID3D11InputLayout*   g_VertexLayout;

    // ファー専用
    ID3D11Buffer*              g_FurParamBuffer;   // cbuffer b7
    ID3D11ShaderResourceView*  g_NoiseSRV;         // ノイズテクスチャ
    ID3D11BlendState*          g_AlphaBlendState;  // αブレンド用
    ID3D11BlendState*          g_NoBlendState;     // αブレンド無効用
    ID3D11RasterizerState*     g_NoCullState;      // 両面描画用

    LIGHT Light;

    // ファーパラメータ
    float FurLength;
    int   ShellCount;
    float FurDensity;

public:
    HRESULT InitPolygonModel(void);
    void FinalizePolygonModel(void);
    void UpdatePolygonModel(void);
    void DrawPolygonModel(void);
};
