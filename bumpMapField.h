/*==============================================================================

   ���_�Ǘ�[polygon3D.h]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#pragma once


#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************


////////////////
//�\����
////////////////


class BumpMapField
{
protected:
	XMFLOAT3	Position;
	XMFLOAT3	Scale;
	XMFLOAT3	Rotate;
	int			TexID;
	int			BumpTexID; // �@���}�b�v�p

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
// �v���g�^�C�v�錾
//*****************************************************************************
