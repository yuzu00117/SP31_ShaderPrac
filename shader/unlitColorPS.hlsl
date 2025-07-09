
#include "common.hlsl"

// Unlit Color Pixel Shader
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse = In.Diffuse;
}

