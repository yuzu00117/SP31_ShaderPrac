/*==============================================================================

   ????[polygon3D.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// ?????
//*****************************************************************************

////////////////
// ???
////////////////

class ProjectionShadowField
{
protected:
	XMFLOAT3	Position;
	XMFLOAT3	Scale;
	XMFLOAT3	Rotate;
	int			TexID;

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* VertexLayout;

public:
	HRESULT InitProjectionShadowField(void);
	void FinalizeProjectionShadowField(void);
	void UpdateProjectionShadowField(void);
	void DrawProjectionShadowField(void);
};

//*****************************************************************************
// ????????
//*****************************************************************************
