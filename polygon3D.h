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


class Object3D
{
	protected:
		XMFLOAT3	Position;
		XMFLOAT3	Scale;
		XMFLOAT3	Rotate;
		int			TexID;


		ID3D11Buffer* VertexBuffer;	//�e�X�g�p���_�o�b�t�@
		ID3D11Buffer* IndexBuffer;	//�e�X�g�p�C���f�b�N�X�o�b�t�@


		ID3D11VertexShader* VertexShader;
		ID3D11PixelShader* PixelShader;
		ID3D11InputLayout* VertexLayout;

	public:
		HRESULT InitPolygon3D(void);
		void FinalizePolygon3D(void);
		void UpdatePolygon3D(void);
		void DrawPolygon3D(void);



};







//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
