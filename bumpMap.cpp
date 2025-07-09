///*==============================================================================
//
//   ���_�Ǘ� [polygon3D.cpp]
//														 Author :
//														 Date   :
//--------------------------------------------------------------------------------
//
//==============================================================================*/
//#include "polygon3D.h"
//#include "sprite.h"
//
//#include "texture.h"
//#include"bumpMap.h"
////*****************************************************************************
//// �}�N����`
////*****************************************************************************
//
//
//
////*****************************************************************************
//// �v���g�^�C�v�錾
////*****************************************************************************
//
//
////*****************************************************************************
//// �O���[�o���ϐ�
////*****************************************************************************
//
////���_�f�[�^
///*
//      V0------------------V1
//      |                  |
//     |                  |
//    |                  |
//   V2-----------------V3
//*/
//
//
//
//
////=============================================================================
//// ����������
////=============================================================================
//HRESULT BumpMapField::InitPolygon3D(void)
//{
//
//	//�e�N�X�`���ǂݍ���
//	int Texture = TextureLoad(L"asset\\texture\\Normal.bmp");
//
//
//	//�V�F�[�_�[�ǂݍ���
//	CreateVertexShader(&VertexShader, &VertexLayout, "UnlitTextureVS.cso");
//	CreatePixelShader(&PixelShader, "UnlitTexturePS.cso");
//	//���_�o�b�t�@�쐬
//	{
//		D3D11_BUFFER_DESC	bd;
//		ZeroMemory(&bd, sizeof(bd));
//		bd.Usage = D3D11_USAGE_DYNAMIC;
//		bd.ByteWidth = sizeof(VERTEX_3D) * BOX_NUM_VERTEX;
//		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
//		GetDevice()->CreateBuffer(&bd, NULL, &VertexBuffer);
//
//		//���_�o�b�t�@�̏������ݐ�̃|�C���^�[���擾
//		D3D11_MAPPED_SUBRESOURCE	msr;
//		GetDeviceContext()->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
//		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;
//
//		//���_�f�[�^���R�s�[
//		CopyMemory(&vertex[0], &Box[0], sizeof(VERTEX_3D) * BOX_NUM_VERTEX);
//		//�������݊���
//		GetDeviceContext()->Unmap(VertexBuffer, 0);
//	}
//
//	//3D�I�u�W�F�N�g�Ǘ��\���̂̏�����
//	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
//	Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
//	Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
//	TexID = Texture;
//
//
//	return S_OK;
//}
//
////=============================================================================
//// �I������
////=============================================================================
//void BumpMapField::FinalizePolygon3D(void)
//{
//	//��������̂����
//
//
//
//	VertexBuffer->Release();
//}
//
////=============================================================================
//// �X�V����
////=============================================================================
//void BumpMapField::UpdatePolygon3D(void)
//{
//
//}
//
////=============================================================================
//// �`�揈��
////=============================================================================
//void BumpMapField::DrawPolygon3D(void)
//{
//	// ���_���C�A�E�g�ݒ�
//	GetDeviceContext()->IASetInputLayout(VertexLayout);
//	//���_�V�F�[�_�[���Z�b�g
//	GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
//	//�s�N�Z���V�F�[�_�[���Z�b�g
//	GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);
//
//	{
//		//�e�N�X�`�����Z�b�g
//		ID3D11ShaderResourceView* tex = GetTexture(TexID);
//		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);
//
//		//���s�ړ��s��쐬
//		XMMATRIX	TranslationMatrix =
//			XMMatrixTranslation(
//				Position.x,
//				Position.y,
//				Position.z
//			);
//		//��]�s��쐬
//		XMMATRIX	RotationMatrix =
//			XMMatrixRotationRollPitchYaw(
//				XMConvertToRadians(Rotate.x),
//				XMConvertToRadians(Rotate.y),
//				XMConvertToRadians(Rotate.z)
//			);
//		//�X�P�[�����O�s��쐬
//		XMMATRIX	ScalingMatrix =
//			XMMatrixScaling(
//				Scale.x,
//				Scale.y,
//				Scale.z
//			);
//		//���[���h�s��쐬 ����Z�̏��Ԃɒ���
//		XMMATRIX	WorldMatrix =
//			ScalingMatrix *
//			RotationMatrix *
//			TranslationMatrix;
//		//DirectX�փZ�b�g
//		SetWorldMatrix(WorldMatrix);
//
//		//���_�o�b�t�@���Z�b�g
//		UINT	stride = sizeof(VERTEX_3D);
//		UINT	offset = 0;
//		GetDeviceContext()->IASetVertexBuffers(
//			0,
//			1,
//			&VertexBuffer,
//			&stride,
//			&offset
//		);
//
//		//�v���~�e�B�u�g�|���W�[�̐ݒ�
//		GetDeviceContext()->IASetPrimitiveTopology(
//			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
//		);
//		//�}�e���A���ݒ�
//		MATERIAL	material;
//		ZeroMemory(&material, sizeof(MATERIAL));
//		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//		SetMaterial(material);
//
//		//�`��
//		GetDeviceContext()->Draw(BOX_NUM_VERTEX, 0);//�C���f�b�N�X�����`��
//	}
//
//
//}
