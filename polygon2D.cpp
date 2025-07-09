/*==============================================================================

   ���_�Ǘ� [polygon2D.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "polygon2D.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

//=============================================================================
// ����������
//=============================================================================
HRESULT Object2D::InitPolygon2D(void)
{
	// �e�N�X�`���ǂݍ���
	int Texture = TextureLoad(L"asset\\texture\\center_white.png");
	TexID = Texture;

	//�V�F�[�_�[�ǂݍ���
	CreateVertexShader(&VertexShader, &VertexLayout, "UnlitTextureVS.cso");
	CreatePixelShader(&PixelShader, "UnlitTexturePS.cso");


	//2D�I�u�W�F�N�g������
	Position = XMFLOAT3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Scale = XMFLOAT2(0.01f, 0.01f);
	Size = XMFLOAT2(SCREEN_WIDTH / 3, SCREEN_WIDTH / 3);
	Rotate = 0.0f;


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Object2D::FinalizePolygon2D(void)
{
	VertexShader->Release();
	PixelShader->Release();
	VertexLayout->Release();
}

//=============================================================================
// �X�V����
//=============================================================================
void Object2D::UpdatePolygon2D(void)
{
	//�K���ɉ�]
	//Rotate += 0.2f;

}

//=============================================================================
// �`�揈��
//=============================================================================
void Object2D::DrawPolygon2D(void)
{

	// ���_���C�A�E�g�ݒ�
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//���_�V�F�[�_�[���Z�b�g
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//�s�N�Z���V�F�[�_�[���Z�b�g
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	//���s��������OFF
	SetDepthEnable(false);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	{//2D�|���S��1�����ŕK�v�ȏ���

		//�e�N�X�`�����Z�b�g
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//���s�ړ��s��̍쐬�i�\�����W�����߂�j
		XMMATRIX	TranslationMatrix = XMMatrixTranslation(
			Position.x, Position.y, 0.0f);

		//��]�s��iZ��]�j�s��̍쐬
		XMMATRIX	RotationMatrix = XMMatrixRotationZ(XMConvertToRadians(Rotate));

		//�X�P�[�����O�s��쐬�i�{��1.0�����{�A0�{�̓_���I�j
		XMMATRIX	ScalingMatrix = XMMatrixScaling(Scale.x,Scale.y, 1.0f);

		//���[���h�s��̍쐬�i�|���S���̕\���̎d�����w�肷��ŏI�I�ȍs��
		XMMATRIX	WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;

		//���[���h�s���DirectX�փZ�b�g
		SetWorldMatrix(WorldMatrix);

		// �|���S���`��
		DrawSprite(Size, Color);
	}


}
