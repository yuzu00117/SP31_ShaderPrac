#include "common.hlsl"
Texture2D g_Texture : register(t0); // �e�N�X�`���O��
SamplerState g_SamplerState : register(s0); // �T���v���[�O��

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
//��������s�N�Z���ւ̃x�N�g��
    float3 lv = normalize(In.WorldPosition.xyz - Light.Position.xyz);

//�����v�Z
    float4 normal = normalize(In.Normal);
    float light = -dot(normal.xyz, lv);

//�����ɂ�錸��
    float3 ld = length(In.WorldPosition.xyz - Light.Position.xyz);
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld;
    ofs = saturate(ofs);
    light *= ofs;

// �����x�N�g��
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

//���˃x�N�g��
    float3 refv = reflect(Light.Direction.xyz, normal.xyz);
    refv = normalize(refv);

//�X�y�L�����̌v�Z
    float specular = -dot(eyev, refv);
    specular = saturate(specular);
    specular = pow(specular, 30.0f);

//�R�[���̌�����lv�̊p�x
    float angle = acos(dot(lv.xyz, normalize(Light.Direction.xyz)));

//angle���R�[���̒��ɂǂ̂��炢�����Ă��邩�H�ɂ�閾�邳�l�i�X�|�b�g���C�g�ł̌����l�j
    float spot = 1.0f - 1.0f / Light.Angle.x * abs(angle);

// ���邳�̒l�Ƀe�N�X�`���ƒ��_�F�ƃA���r�G���g������
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= Light.Diffuse.rgb * In.Diffuse.rgb * light * spot + Light.Ambient.rgb;
    outDiffuse.rgb += (specular * spot);

    outDiffuse.a *= In.Diffuse.a;

}
