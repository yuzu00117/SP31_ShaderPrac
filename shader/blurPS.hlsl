Texture2D SceneTex : register(t0);
SamplerState samLinear : register(s0);

cbuffer BlurParams : register(b8)
{
    float2 Direction; // (1,0) horizontal, (0,1) vertical
    float  Sigma;
    int    Radius;
    float2 TexelSize; // 1/width, 1/height
    float2 _pad;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

// Compute normalized Gaussian weight for offset x given sigma
float Gauss(float x, float sigma)
{
    return exp(-(x*x) / (2.0 * sigma * sigma));
}

float4 ps_main(PSInput i) : SV_Target
{
    // Separable gaussian: sample along Direction
    float2 stepUV = TexelSize * Direction;

    // Accumulate
    float3 acc = 0;
    float  wsum = 0;

    // Center sample
    float w0 = Gauss(0.0, Sigma);
    float3 c0 = SceneTex.Sample(samLinear, i.uv).rgb;
    acc += c0 * w0;
    wsum += w0;

    // Positive/negative pairs
    [unroll(32)]
    for (int k = 1; k <= Radius; ++k)
    {
        float w = Gauss(k, Sigma);
        float2 off = stepUV * k;
        float3 cp = SceneTex.Sample(samLinear, i.uv + off).rgb;
        float3 cn = SceneTex.Sample(samLinear, i.uv - off).rgb;
        acc += (cp + cn) * w;
        wsum += 2.0 * w;
    }

    float3 col = acc / max(wsum, 1e-6);
    return float4(col, 1);
}

// Dummy VS for build systems
struct VSOutDummy { float4 pos : POSITION; float2 uv : TEXCOORD0; };
VSOutDummy main(float4 pos : POSITION, float2 uv : TEXCOORD0)
{
    VSOutDummy o; o.pos = pos; o.uv = uv; return o;
}
