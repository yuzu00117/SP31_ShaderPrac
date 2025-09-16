Texture2D SceneTex : register(t0);
SamplerState samLinear : register(s0);

cbuffer ParameterBuffer : register(b6)
{
    float4 Parameter; // x=ScreenW, y=ScreenH, z=RectSize, w=0
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 main(PSInput input) : SV_Target
{
    float2 texSize = float2(Parameter.x, Parameter.y);
    float rectSize = max(1.0, Parameter.z);

    // UV to pixel position
    float2 pixelPos = input.uv * texSize;

    // Compute top-left of block
    float2 blockPos = floor(pixelPos / rectSize) * rectSize + rectSize * 0.5;

    // Back to UV
    float2 sampleUV = blockPos / texSize;

    return SceneTex.Sample(samLinear, sampleUV);
}
