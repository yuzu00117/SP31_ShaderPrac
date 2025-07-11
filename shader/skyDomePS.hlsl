#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// Sky Dome Pixel Shader
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // ���[���h���W��Y�l�ŃO���f�[�V�������쐬�i���m���ȕ��@�j
    float3 worldPos = normalize(In.WorldPosition.xyz);
    float height = worldPos.y; // -1.0 (��) ���� 1.0 (��) �͈̔�
    
    // 0.0����1.0�Ƀ}�b�s���O
    float gradient = (height + 1.0f) * 0.5f;
    gradient = saturate(gradient); // 0.0-1.0�͈̔͂ɃN�����v
    
    // �㕔�͖��邢�A�����͔����I�����W�F�̃O���f�[�V����
    float4 skyColor = float4(0.5f, 0.8f, 1.0f, 1.0f);      // ���邢��F�i�㕔�j
    float4 horizonColor = float4(1.0f, 0.7f, 0.4f, 1.0f);  // �I�����W�F�i�����j
    
    // �㉺�ŃX���[�Y�Ƀu�����h
    float4 color = lerp(horizonColor, skyColor, gradient);
    
    // �A���t�@�͊��S�s����
    color.a = 1.0f;
    
    outDiffuse = color;
}