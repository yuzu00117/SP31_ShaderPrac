#include "common.hlsl"

// �������C�e�B���O�̃s�N�Z���V�F�[�_�[

Texture2D g_Texture : register(t0); //�e�N�X�`���O��
SamplerState g_SamplerState : register(s0); //�T���v���[�O��
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float4 normal = normalize(In.Normal); // �s�N�Z���̖@���𐳋K��
    float light = -dot(normal.xyz, normalize(Light.Direction.xyz)); // �����v�Z������

    // �e�N�X�`���̃s�N�Z���F���擾
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);

    outDiffuse.rgb *= (In.Diffuse.rgb * light + Light.Ambient.rgb); // ���邳����Z
    outDiffuse.a *= In.Diffuse.a; // ���Ɋ֌W�Ȃ��̂ŕʌv�Z

    // �J��������s�N�Z���֌������x�N�g��
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev); // ���K������

    // �n�[�t�x�N�g���̍쐬
    float3 halfv = eyev + normalize(Light.Direction.xyz); // �����ƃ��C�g�x�N�g�������Z
    halfv = normalize(halfv); // ���K������
    float specular = -dot(halfv, normal.xyz); // �n�[�t�x�N�g���Ɩ@���̓��ς��v�Z
    specular = saturate(specular); // �T�`�����[�g����
    specular = pow(specular, 30);

    // �������C�e�B���O
    float norm = dot(normal, normalize(Light.GroundNormal)); // �n�ʂƃs�N�Z���@���̓���
    norm = (norm + 1.0f) / 2.0f; // ���ς��X�P�[�����O
    float3 hemiColor = lerp(Light.GroundColor, Light.SkyColor, norm); // �F�̕��

    outDiffuse.rgb += specular + hemiColor; // �܂Ƃ߂đ�������
}
