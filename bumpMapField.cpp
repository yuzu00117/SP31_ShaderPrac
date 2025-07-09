/*==============================================================================

   ���_�Ǘ� [bumpMapField.cpp]
								 Author :
								 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include "bumpMapField.h"
#include "sprite.h"

#include "texture.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define	BOX_NUM_VERTEX	(4)
#define	SIZE	(5)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
static LIGHT Light;



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

//���_�f�[�^
/*
	  V0------------------V1
	  |                  |
	 |                  |
	|                  |
   V2-----------------V3
*/

VERTEX_3D	Bump_Box[BOX_NUM_VERTEX] =
{
	//�V��////////////////////////////////////////////
	{//���_V0 LEFT-TOP
		XMFLOAT3(-SIZE / 2, 0.0f, SIZE / 2),	//���_���W
		XMFLOAT3(0.0f,1.0f,0.0f),		//�@��
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//�J���[
		XMFLOAT2(0.0f * SIZE,0.0f * SIZE)				//�e�N�X�`�����W
	},
	{//���_V1 RIGHT-TOP
		XMFLOAT3(SIZE / 2, 0.0f, SIZE / 2),	//���_���W
		XMFLOAT3(0.0f,1.0f,0.0f),		//�@��
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//�J���[
		XMFLOAT2(1.0f * SIZE,0.0f * SIZE)				//�e�N�X�`�����W
	},
	{//���_V2 LEFT-BOTTOM
		XMFLOAT3(-SIZE / 2, 0.0f, -SIZE / 2),	//���_���W
		XMFLOAT3(0.0f,1.0f,0.0f),		//�@��
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//�J���[
		XMFLOAT2(0.0f * SIZE,1.0f * SIZE)				//�e�N�X�`�����W
	},

	{//���_V3 RIGHT-BOTTOM
		XMFLOAT3(SIZE / 2, 0.0f, -SIZE / 2),		//���_���W
		XMFLOAT3(0.0f,1.0f,0.0f),		//�@��
		XMFLOAT4(1.0f,1.0f,1.0f,1.0f),	//�J���[
		XMFLOAT2(1.0f * SIZE,1.0f * SIZE)				//�e�N�X�`�����W
	},

};


//=============================================================================
// ����������
//=============================================================================
HRESULT BumpMapField::InitBumpMapField(void)
{

	//�e�N�X�`���ǂݍ���
	int Texture = TextureLoad(L"asset\\texture\\sura.jpg");
	BumpTexID = TextureLoad(L"asset\\texture\\test.png");

	// �V�F�[�_�[�̓ǂݍ���
	CreateVertexShader(&VertexShader, &VertexLayout, "bumpVS.cso");
	CreatePixelShader(&PixelShader, "bumpPS.cso");

	//���C�g�p�����[�^��ݒ�
	Light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	Light.Ambient = XMFLOAT4(0.5f, 0.3f, 0.3f, 1.0f);
	Light.Direction = XMFLOAT4(0.0f, -1.0f, 1.0f, 0.0f);

	Light.Position = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);//���f���̏�����O������
	Light.PointLightParam = XMFLOAT4(10.0f, 0.0f, 0.0f, 0.0f);//x=����
	Light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);//�A���r�G���g�������

	//CreateVertexShader(&VertexShader, &VertexLayout, "UnlitTextureVS.cso");
	//CreatePixelShader(&PixelShader, "UnlitTexturePS.cso");
	//���_�o�b�t�@�쐬
	{
		D3D11_BUFFER_DESC	bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * BOX_NUM_VERTEX;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		GetDevice()->CreateBuffer(&bd, NULL, &VertexBuffer);

		//���_�o�b�t�@�̏������ݐ�̃|�C���^�[���擾
		D3D11_MAPPED_SUBRESOURCE	msr;
		GetDeviceContext()->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		//���_�f�[�^���R�s�[
		CopyMemory(&vertex[0], &Bump_Box[0], sizeof(VERTEX_3D) * BOX_NUM_VERTEX);
		//�������݊���
		GetDeviceContext()->Unmap(VertexBuffer, 0);
	}

	//3D�I�u�W�F�N�g�Ǘ��\���̂̏�����
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	TexID = Texture;

	////���C�g�p�����[�^�[
	//Light.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	//Light.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);

	//Light.Direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f); //���C�g�̌���
	//Light.Position = XMFLOAT4(0.0f, 2.0f, 0.0f, 0.0f);   //���C�g�̍��W
	//Light.PointLightParam = XMFLOAT4(200.0f, 1.5f, 0.0f, 0.0f); //x = ���̓͂����� y = �␳�l
	//Light.Angle = XMFLOAT4((3.14159f / 180.0f) * 30.0f, 0.0f, 0.0f, 0.0f); //�R�[���p�x

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void BumpMapField::FinalizeBumpMapField(void)
{
	//��������̂����
	VertexBuffer->Release();
}

//=============================================================================
// �X�V����
//=============================================================================
void BumpMapField::UpdateBumpMapField(void)
{
	if (GetAsyncKeyState('E') & 0x8000)
	{
		Light.Position.x += 0.1f;
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		Light.Position.x -= 0.1f;
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void BumpMapField::DrawBumpMapField(void)
{
	SetLight(Light);

	// ���_���C�A�E�g�ݒ�
	GetDeviceContext()->IASetInputLayout(VertexLayout);
	//���_�V�F�[�_�[���Z�b�g
	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
	//�s�N�Z���V�F�[�_�[���Z�b�g
	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

	{
		//�e�N�X�`�����Z�b�g
		ID3D11ShaderResourceView* tex = GetTexture(TexID);
		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

		// �o���v�}�b�v�e�N�X�`�����Z�b�g
		ID3D11ShaderResourceView* bumpTex = GetTexture(BumpTexID);
		GetDeviceContext()->PSSetShaderResources(1, 1, &bumpTex); // register(t1) �Ƀo�C���h

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

		//���_�o�b�t�@���Z�b�g
		UINT	stride = sizeof(VERTEX_3D);
		UINT	offset = 0;
		GetDeviceContext()->IASetVertexBuffers(
			0,
			1,
			&VertexBuffer,
			&stride,
			&offset
		);

		//�v���~�e�B�u�g�|���W�[�̐ݒ�
		GetDeviceContext()->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		);
		//�}�e���A���ݒ�
		MATERIAL	material;
		ZeroMemory(&material, sizeof(MATERIAL));
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);

		//�`��
		GetDeviceContext()->Draw(BOX_NUM_VERTEX, 0);//�C���f�b�N�X�����`��
	}
}
