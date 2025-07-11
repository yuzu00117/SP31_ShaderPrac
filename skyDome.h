#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SKYDOME_RADIUS 100.0f
#define SKYDOME_SEGMENTS 32
#define SKYDOME_RINGS 16

//*****************************************************************************
// �\����
//*****************************************************************************
class SkyDome
{
protected:
    XMFLOAT3    Position;
    XMFLOAT3    Scale;
    XMFLOAT3    Rotate;
    int         TexID;

    ID3D11Buffer* VertexBuffer;    // ���_�o�b�t�@
    ID3D11Buffer* IndexBuffer;     // �C���f�b�N�X�o�b�t�@

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