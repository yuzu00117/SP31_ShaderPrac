#include "common.hlsl"
Texture2D g_Texture : register(t0); // �e�N�X�`���O��
SamplerState g_SamplerState : register(s0); // �T���v���[�O��

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // ��������s�N�Z���ւ̃x�N�g��
    float4 lv = In.WorldPosition - Light.Position;

    // ���̂ƌ����̋���
    float4 ld = length(lv);

    // �x�N�g���̐��K��
    lv = normalize(lv);

    // �����̌v�Z
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld; // �����̌v�Z
    // �����𖢖��ɂ��Ȃ�
    ofs = max(0, ofs);

    // �s�N�Z���̖@���𐳋K��
    float4 normal = normalize(In.Normal);

    // ���ʌv�Z
    float light = dot(normal.xyz, lv.xyz);
    light = saturate(light);
    light *= ofs; // ���邳������������

    // �e�N�X�`���̃s�N�Z���F���擾
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light + Light.Ambient.rgb; // ���邳����Z
    outDiffuse.a *= In.Diffuse.a; // ���l�ɖ��邳�͊֌W�Ȃ�

    // �J��������s�N�Z���֌������x�N�g��
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

    // �n�[�t�x�N�g�����v�Z
    float3 halfv = eyev + lv.xyz; // �����x�N�g���{���C�g�x�N�g��
    halfv = normalize(halfv);

    // �X�y�L�����[�̌v�Z
    float specular = dot(halfv, normal.xyz); // �n�[�t�x�N�g���Ɩ@���̓���
    specular = saturate(specular);
    specular = pow(specular, 30);

    outDiffuse.rgb += (specular * ofs); // �X�y�L���������������Ă�����Z���ďo��
}
