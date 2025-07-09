/*==============================================================================

   レンダリング管理[renderer.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once

#include "main.h"

//*********************************************************
// 構造体
//*********************************************************

// 頂点構造体
struct VERTEX_3D
{
	XMFLOAT3 Position;	//頂点座標　XMFLOAT3 ＝　float x,y,z
	XMFLOAT3 Normal;	//法線ベクトル 
	XMFLOAT4 Diffuse;	//色  XMFLOAT4 = float x,y,z,w
	XMFLOAT2 TexCoord;	//テクスチャ座標 XMFLOAT2 = float x,y
};

// マテリアル構造体
struct MATERIAL
{
	XMFLOAT4	Ambient;	//アンビエント
	XMFLOAT4	Diffuse;	//デフューズ
	XMFLOAT4	Specular;	//スペキュラ
	XMFLOAT4	Emission;	//エミッシブ
	float		Shininess;	//スペキュラパラメータ
	float		Dummy[3];	//16byte境界調整用パディング
};

struct LIGHT
{
	BOOL		Enable;
	BOOL		Dummy[3];//16byte境界用
	XMFLOAT4	Direction;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Ambient;

	XMFLOAT4	Position;//ポイントライトの座標
	XMFLOAT4	PointLightParam;

	XMFLOAT4 Angle;

	XMFLOAT4 SkyColor;//空の色
	XMFLOAT4 GroundColor;//地面の色
	XMFLOAT4 GroundNormal;//地面の法線
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void FinalizeRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device *GetDevice( void );
ID3D11DeviceContext *GetDeviceContext( void );

void SetDepthEnable( bool Enable );

void SetWorldViewProjection2D(void);
void ResetWorldViewProjection3D(void);


void SetWorldMatrix(XMMATRIX WorldMatrix );
void SetViewMatrix(XMMATRIX ViewMatrix );
void SetProjectionMatrix(XMMATRIX ProjectionMatrix );

void SetCameraPosition(XMFLOAT3 CameraPosition);

void SetParameter(XMFLOAT4 Parameter);



void SetMaterial( MATERIAL Material );

void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName);

void SetLight(LIGHT Light);

IDXGISwapChain* GetSwapChain(void);

