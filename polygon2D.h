/*==============================================================================

   ���_�Ǘ�[polygon2D.h]
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

class Object2D
{
	protected:
		XMFLOAT3	Position;		//�\�����W
		XMFLOAT4	Color;			//�F
		XMFLOAT2	Size;			//�T�C�Y
		XMFLOAT2	Scale;			//�g��k����
		float		Rotate;			//��]�p�x�i360�\���j
		int			TexID;			//�e�N�X�`��ID

		ID3D11VertexShader* VertexShader;
		ID3D11PixelShader* PixelShader;
		ID3D11InputLayout* VertexLayout;

	public:
		HRESULT InitPolygon2D(void);
		void FinalizePolygon2D(void);
		void UpdatePolygon2D(void);
		void DrawPolygon2D(void);



};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
