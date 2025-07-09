/*==============================================================================

   �ȈՃX�v���C�g�\��
														 Author :
														 Date   :
--------------------------------------------------------------------------------

==============================================================================*/
#include	"main.h"
#include	"sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define NUM_SPRITEVERTEX 4

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_�o�b�t�@

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSprite(void)
{
	ID3D11Device *pDevice = GetDevice();

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;//���I��������
	bd.ByteWidth = sizeof(VERTEX_3D) * NUM_SPRITEVERTEX;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU�ŏ������݂���
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void FinalizeSprite(void)
{
	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �`�揈��
//====================

void DrawSprite(XMFLOAT2 size, XMFLOAT4 color)
{

	//���_�o�b�t�@�̏������ݐ�|�C���^�[���擾
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float size_W = size.x * 0.5f;
	float size_H = size.y * 0.5f;

	//���� ���S����T�C�Y�̔����̏ꏊ�ō��W�����
	vertex[0].Position = XMFLOAT3(-size_W, -size_H, 0.0f);
	vertex[0].Diffuse = color;
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	//�E�� ���S����T�C�Y�̔����̏ꏊ�ō��W�����
	vertex[1].Position = XMFLOAT3(size_W, -size_H, 0.0f);
	vertex[1].Diffuse = color;
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	//���� ���S����T�C�Y�̔����̏ꏊ�ō��W�����
	vertex[2].Position = XMFLOAT3(-size_W, size_H, 0.0f);
	vertex[2].Diffuse = color;
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	//�E�� ���S����T�C�Y�̔����̏ꏊ�ō��W�����
	vertex[3].Position = XMFLOAT3(size_W, size_H, 0.0f);
	vertex[3].Diffuse = color;
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);
	//�������݊���
	GetDeviceContext()->Unmap(g_VertexBuffer, 0);


	// ���_�o�b�t�@��DirectX�փZ�b�g
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// �}�e���A�����V�F�[�_�[�֑���
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);
	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}

