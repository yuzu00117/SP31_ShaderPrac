Texture2D SceneTex : register(t0);
SamplerState samLinear : register(s0);

cbuffer BloomPrefilter : register(b9)
{
    float Threshold;
    float SoftKnee;
    float2 _pad0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float Luma(float3 c) { return dot(c, float3(0.2126, 0.7152, 0.0722)); }

float4 ps_main(PSInput i) : SV_Target
{
    float3 col = SceneTex.Sample(samLinear, i.uv).rgb;
    float luma = Luma(col);

    float knee = Threshold * SoftKnee;
    float w = smoothstep(Threshold - knee, Threshold + knee, luma);
    float3 bright = col * saturate(w);

    return float4(bright, 1.0);
}

// Dummy VS for build systems
struct VSOutDummy { float4 pos : POSITION; float2 uv : TEXCOORD0; };
VSOutDummy main(float4 pos : POSITION, float2 uv : TEXCOORD0)
{
    VSOutDummy o; o.pos = pos; o.uv = uv; return o;
}
