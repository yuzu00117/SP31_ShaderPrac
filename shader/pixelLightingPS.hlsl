#include "common.hlsl"

// �s�N�Z�����C�e�B���O�̃s�N�Z���V�F�[�_�[

Texture2D g_Texture : register(t0);//�e�N�X�`���O��
SamplerState g_SamplerState : register(s0);//
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    
      // �s�N�Z���̖@���𐳋K��
    float4 normal = normalize(In.Normal);
    float light = -dot(normal.xyz, Light.Direction.xyz); // �����v�Z������

    // �e�N�X�`���̃s�N�Z���F���擾
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light; // ���邳����Z
    outDiffuse.a *= In.Diffuse.a; // ���ɖ��邳�͊֌W�Ȃ��̂ŕʌv�Z

    // �J��������s�N�Z���֌������x�N�g��
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev); // ���K������

    // ���̔��˃x�N�g�����v�Z
    float3 refv = reflect(Light.Direction.xyz, normal.xyz);
    refv = normalize(refv); // ���K������

    float specular = -dot(eyev, refv); // ���ʔ��˂̌v�Z
    specular = saturate(specular); // �l���T�`�����[�g
    specular = pow(specular, 30); // �����ł�30���悵�Ă݂�
    
    outDiffuse.rgb += specular;//�X�y�L�����l���łЂ�[���Ƃ��đ�������
}