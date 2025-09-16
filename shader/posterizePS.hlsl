Texture2D SceneTex : register(t0);
SamplerState samLinear : register(s0);

// Posterize parameters. Keep values in 0..1 for color processing.
// Levels >= 2; ContrastPower = 1 disables; GrayScaleEnabled 0/1.
cbuffer PosterizeParams : register(b8)
{
    int   Levels;              // number of steps per channel (>=2), default 4
    float ContrastPower;       // 1.0 = no change, >1 increases contrast
    float GrayScaleEnabled;    // 0/1: convert to grayscale first when 1
    float pad;                 // align to 16 bytes
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float3 ToGray(float3 c)
{
    // Rec. 709 luma weights
    float g = dot(c, float3(0.2126, 0.7152, 0.0722));
    return g.xxx;
}

float3 Posterize(float3 c, int levels)
{
    // 各RGBチャンネルを0?1の範囲で段階化します。
    // 段階数を上げるとオリジナルに近づき、下げると減色が強くなります。
    levels = max(levels, 2);
    // 0と1を含む段階に丸める実装
    float3 q = floor(c * levels) / (levels - 1);
    return saturate(q);
}

// Pixel shader entry point (runtime compiles with entry = "ps_main")
float4 ps_main(PSInput input) : SV_Target
{
    float4 col = SceneTex.Sample(samLinear, input.uv);

    if (GrayScaleEnabled > 0.5)
    {
        col.rgb = ToGray(col.rgb);
    }

    col.rgb = Posterize(col.rgb, Levels);

    // Optional contrast adjustment in linear 0..1
    if (abs(ContrastPower - 1.0) > 1e-5)
    {
        col.rgb = pow(saturate(col.rgb), max(0.0001, ContrastPower));
    }

    return col;
}

// Dummy VS named 'main' to allow build systems that try to compile this file as a VS.
struct VSOutDummy { float4 pos : POSITION; float2 uv : TEXCOORD0; };
VSOutDummy main(float4 pos : POSITION, float2 uv : TEXCOORD0)
{
    VSOutDummy o; o.pos = pos; o.uv = uv; return o;
}
