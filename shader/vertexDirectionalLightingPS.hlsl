#include "common.hlsl"



Texture2D g_Texture : register(t0); //�e�N�X�`���O��
SamplerState g_SamplerState : register(s0); //�T���v���[�O��

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord); //�e�N�X�`���̐F���擾
    outDiffuse *= In.Diffuse;//�f�t���[�Y�i���_�̖��邳�j������
}