#include"common.hlsl"

Texture2D g_Texture : register(t0); //�e�N�X�`���O��
SamplerState g_SamplerState : register(s0); //�T���v���[�O��
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
 // ��������s�N�Z���ւ̃x�N�g��
    float3 lv = In.WorldPosition.xyz - Light.Position.xyz;

    // �x�N�g���̐��K��
    lv = normalize(lv);

    // �s�N�Z���̖@���𐳋K��
    float3 normal = normalize(In.Normal.xyz);

    // �����v�Z������i�f�B�t���[�Y���j
    float light = -dot(normal, Light.Direction.xyz);
    light = saturate(light); // ���邳�� 0 - 1 �Ɏ��߂�

    // �e�N�X�`���̃s�N�Z���F���擾
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);

    // �f�B�t���[�Y�J���[�Ɍ�����Z
    outDiffuse.rgb *= In.Diffuse.rgb * light;

    // �A���t�@�͕ʓr��Z
    outDiffuse.a *= In.Diffuse.a;

    // �J��������s�N�Z���֌������x�N�g��
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

    // �n�[�t�x�N�g���̍쐬
    float3 halfv = normalize(eyev + Light.Direction.xyz);

    // �X�y�L�����[�̌v�Z
    float specular = -dot(halfv, normal);
    specular = saturate(specular);
    specular = pow(specular, 30);

    float ofs = 1.0f; // �X�y�L�����[���x�W��
    outDiffuse.rgb += (specular * ofs);

    // --- �������C�e�B���O ---

    // ���̕����Ǝ����x�N�g���̍l���i�t���قǖ��邢�j
    float lit = 1.0f - max(0, dot(lv, eyev));

    // �֊s�����قǖ��邭����
    float lim = 1.0f - max(0, dot(normal, -eyev));

    // lit �� lim �̖��邳������
    lim *= lit;
    lim = pow(lim, 3);

    // �������C�g�̃J���[�Ƌ�����ݒ�
    float3 rimColor = float3(1.0, 1.0, 1.0); // �����������C�g
    float rimIntensity = 0.5f; // �����i0.0?1.0�j

    // �������C�e�B���O�̖��邳���f�t���[�Y�ɉ��Z
    outDiffuse.rgb += lim * rimColor * rimIntensity;
}