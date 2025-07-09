#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp); // ���_�ϊ�

    // ���_�@�������[���h�s��ŕϊ����ďo��
    float4 worldNormal, normal;
    normal = float4(In.Normal.xyz, 0.0f);
    worldNormal = mul(normal, World);
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;

    Out.Diffuse = In.Diffuse; // ���_�F�̏o��
    Out.TexCoord = In.TexCoord; // �e�N�X�`�����W�̏o��

    // ���[���h�ϊ��������_���W���o��
    Out.WorldPosition = mul(In.Position, World);
}
