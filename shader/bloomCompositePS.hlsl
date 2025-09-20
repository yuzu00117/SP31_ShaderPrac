Texture2D SceneTex : register(t0);
Texture2D BloomTex : register(t1);
SamplerState samLinear : register(s0);

cbuffer BloomComposite : register(b10)
{
    float Intensity;
    float3 _pad;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 ps_main(PSInput i) : SV_Target
{
    float3 scene = SceneTex.Sample(samLinear, i.uv).rgb;
    float3 bloom = BloomTex.Sample(samLinear, i.uv).rgb;
    float3 outCol = scene + bloom * Intensity;
    return float4(outCol, 1.0);
}

// Dummy VS for build systems
struct VSOutDummy { float4 pos : POSITION; float2 uv : TEXCOORD0; };
VSOutDummy main(float4 pos : POSITION, float2 uv : TEXCOORD0)
{
    VSOutDummy o; o.pos = pos; o.uv = uv; return o;
}
