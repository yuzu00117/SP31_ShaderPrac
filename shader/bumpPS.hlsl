#include "common.hlsl"

Texture2D g_Texture : register(t0); // �e�N�X�`���O��
Texture2D g_TextureNormal : register(t1); // �e�N�X�`���P�ԁi�@���}�b�v�j
SamplerState g_SamplerState : register(s0); // �T���v���[�O��

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // ���̂�������ւ̃x�N�g��
    float4 lv = In.WorldPosition - Light.Position;
    
    // ���̂ƌ����̋���
    float4 ld = length(lv);
    
    // �x�N�g���̐��K��
    lv = normalize(lv);
    
    // �����̌v�Z
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld;
    // �������̕��l��0�ɂ���
    ofs = max(0, ofs);
    
    // �@���}�b�v�擾
    float4 normalMap = g_TextureNormal.Sample(g_SamplerState, In.TexCoord);
    
    // �F�f�[�^���x�N�g���ɓW�J (0-1�͈̔͂�-1�`1�͈̔͂ɕϊ�)
    normalMap = (normalMap * 2.0f) - 1.0f;
    
    // �@���Ƃ��Ċi�[���Ȃ���
    float4 normal;
    normal.x = normalMap.x;
    normal.y = normalMap.y;
    normal.z = normalMap.z;
    normal.w = 0.0f;
    
    // �s�N�Z���̖@���𐳋K��
    normal = normalize(normal);
    
    // �����v�Z�i���ς̕������폜�j
    float light = dot(normal.xyz, lv.xyz);
    //light = saturate(light);
    light *= ofs; // ���邳������
    
    // �e�N�X�`���̃s�N�Z���F���擾
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light; // ���邳����Z
    outDiffuse.a *= In.Diffuse.a; // ���l�ɖ��邳�͊֌W�Ȃ�
    outDiffuse.rgb += Light.Ambient.rgb; // �����̉��Z
    
    // �J��������s�N�Z���֌������x�N�g��
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    // �n�[�t�x�N�g�����v�Z
    float3 halfv = eyev + lv.xyz;
    halfv = normalize(halfv);
    
    // �X�y�L�����[�̌v�Z
    float specular = dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, 30);
    
    outDiffuse.rgb += (specular * ofs);
}