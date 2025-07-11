#include "skyDome.h"
#include "sprite.h"
#include "Camera.h"
#include "texture.h"
#include "renderer.h"
#include <vector>

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
HRESULT SkyDome::InitSkyDome(void)
{
    // �V�F�[�_�[�ǂݍ���
    CreateVertexShader(&VertexShader, &VertexLayout, "skyDomeVS.cso");
    CreatePixelShader(&PixelShader, "skyDomePS.cso");

    // �X�J�C�h�[���̃W�I���g�����쐬
    CreateSphereGeometry();

    // 3D�I�u�W�F�N�g�Ǘ��\���̂̏�����
    Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
    Scale = XMFLOAT3(50.0f, 50.0f, 50.0f); // �X�P�[����傫�����ĉ���������

    // �e�N�X�`���͎g�p���Ȃ��i�V�F�[�_�[�ŃO���f�[�V������`��j
    TexID = -1;

    return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void SkyDome::FinalizeSkyDome(void)
{
    if (VertexBuffer) VertexBuffer->Release();
    if (IndexBuffer) IndexBuffer->Release();
    if (VertexShader) VertexShader->Release();
    if (PixelShader) PixelShader->Release();
    if (VertexLayout) VertexLayout->Release();
}

//=============================================================================
// �X�V����
//=============================================================================
void SkyDome::UpdateSkyDome(void)
{
    // �X�J�C�h�[���͒ʏ��]�������肵�Ȃ�
    // �K�v�ɉ����Ă����ŉ�]������ǉ�
}

//=============================================================================
// �`�揈��
//=============================================================================
void SkyDome::DrawSkyDome(void)
{
    // �[�x�e�X�g�͗L���ɂ��邪�A�[�x�������݂𖳌��ɂ���
    SetDepthEnable(true);
    
    // �[�x�������ݖ����̃X�e�[�g���쐬
    ID3D11DepthStencilState* depthState;
    D3D11_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // �[�x�������ݖ���
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthDesc.StencilEnable = FALSE;
    
    GetDevice()->CreateDepthStencilState(&depthDesc, &depthState);
    GetDeviceContext()->OMSetDepthStencilState(depthState, 0);

    // �J�����O�𖳌��ɂ���i���̓������m���Ɍ��邽�߁j
    ID3D11RasterizerState* rasterizerState;
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE; // �J�����O����
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;

    GetDevice()->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    GetDeviceContext()->RSSetState(rasterizerState);

    // ���_���C�A�E�g�ݒ�
    GetDeviceContext()->IASetInputLayout(VertexLayout);
    // ���_�V�F�[�_�[�ݒ�
    GetDeviceContext()->VSSetShader(VertexShader, NULL, 0);
    // �s�N�Z���V�F�[�_�[�ݒ�
    GetDeviceContext()->PSSetShader(PixelShader, NULL, 0);

    {
        // �e�N�X�`���͎g�p���Ȃ��i�V�F�[�_�[�ŃO���f�[�V�����`��j

        // ���s�ړ��s��쐬�i�J�����ɒǏ]�j
        XMMATRIX TranslationMatrix = XMMatrixTranslation(Position.x, Position.y, Position.z);
        // ��]�s��쐬
        XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(
            XMConvertToRadians(Rotate.x),
            XMConvertToRadians(Rotate.y),
            XMConvertToRadians(Rotate.z)
        );
        // �X�P�[�����O�s��쐬
        XMMATRIX ScalingMatrix = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
        // ���[���h�s��쐬
        XMMATRIX WorldMatrix = ScalingMatrix * RotationMatrix * TranslationMatrix;
        // DirectX�ɐݒ�
        SetWorldMatrix(WorldMatrix);

        // ���_�o�b�t�@�ݒ�
        UINT stride = sizeof(VERTEX_3D);
        UINT offset = 0;
        GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
        GetDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        // �v���~�e�B�u�g�|���W�[�ݒ�
        GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // �}�e���A���ݒ�
        MATERIAL material;
        ZeroMemory(&material, sizeof(MATERIAL));
        material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        SetMaterial(material);

        // �`��
        GetDeviceContext()->DrawIndexed(IndexCount, 0, 0);
    }

    // ��Ԃ����ɖ߂�
    if (rasterizerState) rasterizerState->Release();
    if (depthState) depthState->Release();
    
    // �ʏ�̐[�x�ݒ�ɖ߂�
    SetDepthEnable(true);
}

//=============================================================================
// ���̃W�I���g���쐬
//=============================================================================
void SkyDome::CreateSphereGeometry(void)
{
    std::vector<VERTEX_3D> vertices;
    std::vector<UINT> indices;

    // ���̂̒��_�𐶐�
    for (int ring = 0; ring <= SKYDOME_RINGS; ring++)
    {
        float phi = XM_PI * ring / SKYDOME_RINGS;
        for (int segment = 0; segment <= SKYDOME_SEGMENTS; segment++)
        {
            float theta = 2.0f * XM_PI * segment / SKYDOME_SEGMENTS;

            VERTEX_3D vertex;
            vertex.Position.x = SKYDOME_RADIUS * sinf(phi) * cosf(theta);
            vertex.Position.y = SKYDOME_RADIUS * cosf(phi);
            vertex.Position.z = SKYDOME_RADIUS * sinf(phi) * sinf(theta);

            // �@���͒��S����O����
            vertex.Normal.x = vertex.Position.x / SKYDOME_RADIUS;
            vertex.Normal.y = vertex.Position.y / SKYDOME_RADIUS;
            vertex.Normal.z = vertex.Position.z / SKYDOME_RADIUS;

            vertex.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            
            // �e�N�X�`�����W��K�؂ɐݒ�
            vertex.TexCoord.x = (float)segment / SKYDOME_SEGMENTS;
            vertex.TexCoord.y = (float)ring / SKYDOME_RINGS;

            vertices.push_back(vertex);
        }
    }

    // �C���f�b�N�X�𐶐��i�������猩�邽�߂ɏ������t�ɂ���j
    for (int ring = 0; ring < SKYDOME_RINGS; ring++)
    {
        for (int segment = 0; segment < SKYDOME_SEGMENTS; segment++)
        {
            int current = ring * (SKYDOME_SEGMENTS + 1) + segment;
            int next = current + SKYDOME_SEGMENTS + 1;

            // �O�p�`1�i�������猩�邽�߂ɒ��_�������t�ɂ���j
            indices.push_back(current);
            indices.push_back(current + 1);
            indices.push_back(next);

            // �O�p�`2
            indices.push_back(current + 1);
            indices.push_back(next + 1);
            indices.push_back(next);
        }
    }

    VertexCount = vertices.size();
    IndexCount = indices.size();

    // ���_�o�b�t�@�쐬
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(VERTEX_3D) * VertexCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices.data();
    GetDevice()->CreateBuffer(&bd, &InitData, &VertexBuffer);

    // �C���f�b�N�X�o�b�t�@�쐬
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(UINT) * IndexCount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices.data();
    GetDevice()->CreateBuffer(&bd, &InitData, &IndexBuffer);
}