#include "common.hlsl"

// Sky Dome Vertex Shader
void main(in VS_IN In, out PS_IN Out)
{
    // �J�����ʒu�𒆐S�ɃX�J�C�h�[����z�u
    float4 worldPos = In.Position + float4(CameraPosition.xyz, 0.0f);
    
    Out.WorldPosition = mul(worldPos, World);
    Out.Position = mul(Out.WorldPosition, View);
    Out.Position = mul(Out.Position, Projection);
    
    // �[�x���ő�l�ɂ���i�X�J�C�h�[���͍ł������j
    // z/w = 1.0 �ɂȂ�悤��z��w�ɐݒ�
    Out.Position.z = Out.Position.w * 0.9999f; // ���S��1.0���Ɩ�肪�N����ꍇ������̂ŏ���������
    
    Out.Normal = In.Normal;
    Out.Diffuse = In.Diffuse;
    Out.TexCoord = In.TexCoord;
}