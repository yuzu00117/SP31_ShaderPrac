#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SKYDOME_RADIUS 100.0f
#define SKYDOME_SEGMENTS 32
#define SKYDOME_RINGS 16

//*****************************************************************************
// 構造体
//*****************************************************************************
class SkyDome
{
protected:
    XMFLOAT3    Position;
    XMFLOAT3    Scale;
    XMFLOAT3    Rotate;
    int         TexID;

    ID3D11Buffer* VertexBuffer;    // 頂点バッファ
    ID3D11Buffer* IndexBuffer;     // インデックスバッファ

    ID3D11VertexShader* VertexShader;
    ID3D11PixelShader* PixelShader;
    ID3D11InputLayout* VertexLayout;

    int VertexCount;
    int IndexCount;

public:
    HRESULT InitSkyDome(void);
    void FinalizeSkyDome(void);
    void UpdateSkyDome(void);
    void DrawSkyDome(void);

private:
    void CreateSphereGeometry(void);
};