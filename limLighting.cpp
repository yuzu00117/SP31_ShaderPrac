/*==============================================================================

   ���_�Ǘ� [polygon3D.cpp]
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include"limLighting.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"
#include "model.h"
#include"renderer.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
//�J�����͕�������ƃ_���Ȃ̂ŊO��

static LIGHT Light;

static ID3D11VertexShader* g_VertexShader = NULL;
static ID3D11InputLayout* g_VertexLayout = NULL;
static ID3D11PixelShader* g_PixelShader = NULL;

//��������


//=============================================================================
// ����������
//=============================================================================
HRESULT LimLightingModel::InitPolygonModel(void)
{

	//�V�F�[�_�[�ǂݍ���
	CreateVertexShader(&g_VertexShader, &g_VertexLayout, "limLightingVS.cso");
	CreatePixelShader(&g_PixelShader, "limLightingPS.cso");

	//���C�g�p�����[�^��ݒ�


	Light.Direction = XMFLOAT4(-0.5f, -1.0f, 0.5f, 0.0f);
	Light.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	Light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f); // �^�ォ����𓖂Ă�iRim���o�₷���j



	//3D�I�u�W�F�N�g�Ǘ��\���̂̏�����
	Position = XMFLOAT3(-0.1f, 0.2f, 0.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);


	//���f���ǂݍ���
	Model = ModelLoad("asset\\model\\model.fbx");




	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void LimLightingModel::FinalizePolygonModel(void)
{
	//��������̂����



	ModelRelease(Model);

}

//=============================================================================
// �X�V����
//=============================================================================
void LimLightingModel::UpdatePolygonModel(void)
{
	//�K���ɉ�]
	Rotate.y += 1.3f;
	Rotate.x += 0.1f;

}

//=============================================================================
// �`�揈��
//=============================================================================
void LimLightingModel::DrawPolygonModel(void)
{
	SetLight(Light);


	//---------------------------------------
	//���_���C�A�E�g��ݒ�
	GetDeviceContext()->IASetInputLayout(g_VertexLayout);
	//���_�V�F�[�_�[���Z�b�g
	GetDeviceContext()->VSSetShader(g_VertexShader, NULL, 0);
	//�s�N�Z���V�F�[�_�[���Z�b�g
	GetDeviceContext()->PSSetShader(g_PixelShader, NULL, 0);

	//3D�p�s��ݒ�
	//ResetWorldViewProjection3D();//3D�`��ɐ؂�ւ���
	SetDepthEnable(true);		//���s�������L��

	//�J�����͋��L�̏�񂪐ݒ肳��Ă���Ƃ���

	{//3D�|���S���P���̏���
		//�e�N�X�`�����Z�b�g
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		//���s�ړ��s��쐬
		XMMATRIX	TranslationMatrix =
			XMMatrixTranslation(
				Position.x,
				Position.y,
				Position.z
			);
		//��]�s��쐬
		XMMATRIX	RotationMatrix =
			XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(Rotate.x),
				XMConvertToRadians(Rotate.y),
				XMConvertToRadians(Rotate.z)
			);
		//�X�P�[�����O�s��쐬
		XMMATRIX	ScalingMatrix =
			XMMatrixScaling(
				Scale.x,
				Scale.y,
				Scale.z
			);
		//���[���h�s��쐬 ����Z�̏��Ԃɒ���
		XMMATRIX	WorldMatrix =
			ScalingMatrix *
			RotationMatrix *
			TranslationMatrix;
		//DirectX�փZ�b�g
		SetWorldMatrix(WorldMatrix);

		//�v���~�e�B�u�g�|���W�[�̐ݒ�
		GetDeviceContext()->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		);
		//�}�e���A���ݒ�
		MATERIAL	material;
		ZeroMemory(&material, sizeof(MATERIAL));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//�`��
		ModelDraw(Model);
	}


}
